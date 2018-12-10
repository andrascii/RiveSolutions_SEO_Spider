#include "multi_socket_download_handler.h"
#include "random_interval_range_timer.h"
#include "multi_socket_loader.h"
#include "download_request.h"
#include "download_response.h"
#include "helpers.h"
#include "thread_message_dispatcher.h"

namespace
{

constexpr int c_maxParallelTransferCount = 24;

}

namespace CrawlerEngine
{

MultiSocketDownloadHandler::MultiSocketDownloadHandler()
	: m_multiSocketLoader(new MultiSocketLoader(this))
{
	qRegisterMetaType<ResponseHeaders>("ResponseHeaders");
	qRegisterMetaType<Common::StatusCode>("Common::StatusCode");

	// must be queued connection
	// otherwise loader can emit signal about loaded url
	// before we add the request id to the m_activeRequesters map
	// in this case we have not a chance to find a requester in the onUrlLoaded slot!!!
	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::loaded,
		this, &MultiSocketDownloadHandler::onUrlLoaded, Qt::QueuedConnection));

	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::currentParallelTransferCountChanged,
		this, &MultiSocketDownloadHandler::onCurrentParallelTransfersCountChanged));

	//
	// TODO: optimize the many calls by these connections
	//

//	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::uploadProgress,
//		this, &MultiSocketDownloadHandler::onAboutUploadProgress, Qt::QueuedConnection));
//
//	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::downloadProgress,
//		this, &MultiSocketDownloadHandler::onAboutDownloadProgress, Qt::QueuedConnection));
}

void MultiSocketDownloadHandler::setUserAgent(const QByteArray& userAgent)
{
	m_multiSocketLoader->setUserAgent(userAgent);
}

void MultiSocketDownloadHandler::setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword)
{
	m_multiSocketLoader->setProxySettings(proxyHostName, proxyPort, proxyUser, proxyPassword);
}

void MultiSocketDownloadHandler::resetProxy()
{
	m_multiSocketLoader->resetProxySettings();
}

void MultiSocketDownloadHandler::setTimeout(int msecs)
{
	m_multiSocketLoader->setTimeout(msecs);
}

void MultiSocketDownloadHandler::stopRequestHandling(RequesterSharedPtr requester)
{
	return void();
}

QObject* MultiSocketDownloadHandler::qobject()
{
	return this;
}

std::shared_ptr<DownloadResponse> MultiSocketDownloadHandler::responseFor(int requestId)
{
	if (!m_responses.contains(requestId))
	{
		const int parentId = parentIdFor(requestId);

		if (!m_responses.contains(parentId))
		{
			m_responses[requestId] = std::make_shared<DownloadResponse>();
		}
		else
		{
			return m_responses[parentId];
		}
	}

	return m_responses[requestId];
}

void MultiSocketDownloadHandler::pauseRequesters(const QList<Requester*>& requesterToBePaused)
{
	std::for_each(requesterToBePaused.begin(), requesterToBePaused.end(),
		[this](Requester* p) { m_pausedRequesters.insert(p); });

	// some of these requests at this moment can be already downloaded - it's a normal situation.
	// MultiSocketLoader emit a signal about new downloaded page but our slot onUrlLoaded had not been called yet
	// because of we uses the Qt::QueuedConnection
	const QVector<int> requestIndexes = requestIndexesToPause();

	std::for_each(requestIndexes.begin(), requestIndexes.end(),
		[this](int id) { m_multiSocketLoader->pauseConnection(id); });
}

void MultiSocketDownloadHandler::unpauseRequesters(const QList<Requester*>& requesterToBeUnpaused)
{
	const QVector<int> requestIndexes = requestIndexesToUnpause(requesterToBeUnpaused);

	std::for_each(requesterToBeUnpaused.begin(), requesterToBeUnpaused.end(),
		[this](Requester* p) { m_pausedRequesters.remove(p); });

	if (requestIndexes.isEmpty())
	{
		// In this case MultiSocketDownloadHandler can stay in the sleeping state.
		// We need to make sure that we actually don't have any requesters
		// by checking and extracting requesters from the m_pendingRequesters.
		RequesterSharedPtr requester = extractFirstUnpausedRequester();

		if (!requester)
		{
			INFOLOG << "There is no requesters to unpause and start to load";
			return;
		}

		load(requester);
	}
	else
	{
		std::for_each(requestIndexes.begin(), requestIndexes.end(),
			[this](int id) { m_multiSocketLoader->unpauseConnection(id); });
	}
}

//! extracts and returns the value of the Location header (resolves the location address by baseAddress)
Url MultiSocketDownloadHandler::redirectedUrl(const ResponseHeaders& responseHeaders, const Url& baseAddress) const
{
	const std::vector<QString> locationHeaderData = responseHeaders.valueOf("location");

	Url redirectAddress;

	// supposed that we've got only one location header
	// so we use locationHeaderData[0] to get the redirect url
	if (!locationHeaderData.empty())
	{
		redirectAddress = locationHeaderData[0];

		if (redirectAddress.isValid() && redirectAddress.isRelative())
		{
			redirectAddress = PageParserHelpers::resolveRelativeUrl(redirectAddress, baseAddress);
		}
	}

	return redirectAddress;
}

RequesterSharedPtr MultiSocketDownloadHandler::requesterByIdAssertIfNotExists(int id) const
{
	const auto requesterIterator = m_activeRequesters.find(id);

	if (requesterIterator == m_activeRequesters.end())
	{
		const int parentId = parentIdFor(id);

		DEBUG_ASSERT(parentId != -1 || !"Parent ID not found");

		return m_activeRequesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

RequesterSharedPtr MultiSocketDownloadHandler::requesterById(int id) const
{
	const auto requesterIterator = m_activeRequesters.find(id);

	if (requesterIterator == m_activeRequesters.end())
	{
		const int parentId = parentIdFor(id);

		if (parentId == -1)
		{
			return RequesterSharedPtr();
		}

		return m_activeRequesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

int MultiSocketDownloadHandler::parentIdFor(int id) const
{
	int parendIdSearchResult = -1;

	auto redirectBindingIterator = m_redirectRequestIdToParentId.find(id);

	while (redirectBindingIterator != m_redirectRequestIdToParentId.end())
	{
		parendIdSearchResult = redirectBindingIterator.value();
		redirectBindingIterator = m_redirectRequestIdToParentId.find(parendIdSearchResult);
	}

	return parendIdSearchResult;
}

void MultiSocketDownloadHandler::removeRequestIndexesChain(int id)
{
	std::vector<int> chain;

	auto redirectBindingIterator = m_redirectRequestIdToParentId.find(id);

	while (redirectBindingIterator != m_redirectRequestIdToParentId.end())
	{
		chain.push_back(redirectBindingIterator.key());
		redirectBindingIterator = m_redirectRequestIdToParentId.find(redirectBindingIterator.value());
	}

	if (chain.empty())
	{
		return;
	}

	m_parentIdToRedirectIds.remove(m_redirectRequestIdToParentId[chain.back()]);

	std::for_each(chain.begin(), chain.end(), [this](int id)
	{
		m_redirectRequestIdToParentId.remove(id);
	});
}

//! creates new request to load a redirection target
void MultiSocketDownloadHandler::followLocation(DownloadRequest::BodyProcessingCommand bodyProcessingCommand,
	int parentRequestId,
	const Url& redirectUrlAddress,
	DownloadRequestType requestType)
{
	const CrawlerRequest redirectKey{ redirectUrlAddress, requestType };
	const int redirectionRequestId = loadHelper(redirectKey, bodyProcessingCommand);
	m_redirectRequestIdToParentId[redirectionRequestId] = parentRequestId;
	m_parentIdToRedirectIds[parentRequestId].append(redirectionRequestId);
}

//! true if redirectUrlAddress contains twice in the hopsChain
bool MultiSocketDownloadHandler::isRedirectLoop(const HopsChain& hopsChain, const Url& redirectUrlAddress) const
{
	const auto urlComparator = [&redirectUrlAddress](const Hop& hop)
	{
		return hop.url() == redirectUrlAddress;
	};

	const int urlCount = std::count_if(hopsChain.begin(), hopsChain.end(), urlComparator);

	return urlCount > 2;
}

//! returns the valid request indexes which should be paused
QVector<int> MultiSocketDownloadHandler::requestIndexesToPause() const
{
	using ActiveRequesterMapIterator = QMap<int, RequesterWeakPtr>::const_iterator;

	QVector<int> result;

	for (ActiveRequesterMapIterator iterator = m_activeRequesters.begin(), last = m_activeRequesters.end(); iterator != last; ++iterator)
	{
		if (iterator.value().expired())
		{
			continue;
		}

		RequesterSharedPtr actualRequester = iterator.value().lock();

		if (!m_pausedRequesters.contains(actualRequester.get()))
		{
			continue;
		}

		if (m_parentIdToRedirectIds.contains(iterator.key()))
		{
			DEBUG_ASSERT(!m_parentIdToRedirectIds[iterator.key()].isEmpty());
			result.append(m_parentIdToRedirectIds[iterator.key()].back());
		}
		else
		{
			result.append(iterator.key());
		}
	}

	return result;
}

//! returns the valid request indexes which should be unpaused and stored in the m_activeRequesters and m_pendingRequesters
QVector<int> MultiSocketDownloadHandler::requestIndexesToUnpause(const QList<Requester*>& requesterToBeUnpaused) const
{
	using ActiveRequesterMapIterator = QMap<int, RequesterWeakPtr>::const_iterator;

	QVector<int> result;

	for (ActiveRequesterMapIterator iterator = m_activeRequesters.begin(), last = m_activeRequesters.end(); iterator != last; ++iterator)
	{
		if (iterator.value().expired())
		{
			continue;
		}

		RequesterSharedPtr actualRequester = iterator.value().lock();

		if (requesterToBeUnpaused.indexOf(actualRequester.get()) == -1)
		{
			continue;
		}

		if (m_parentIdToRedirectIds.contains(iterator.key()))
		{
			DEBUG_ASSERT(!m_parentIdToRedirectIds[iterator.key()].isEmpty());
			result.append(m_parentIdToRedirectIds[iterator.key()].back());
		}
		else
		{
			result.append(iterator.key());
		}
	}

	return result;
}

void MultiSocketDownloadHandler::onAboutDownloadProgress(int id, double bytesTotal, double bytesReceived)
{
	const RequesterSharedPtr requester = requesterById(id);

	if (!requester)
	{
		return;
	}

	const std::shared_ptr<DownloadProgressResponse> downloadProgressResponse =
		std::make_shared<DownloadProgressResponse>();

	downloadProgressResponse->bytesReceived = static_cast<quint64>(bytesReceived);
	downloadProgressResponse->bytesTotal = static_cast<quint64>(bytesTotal);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, downloadProgressResponse);
}

void MultiSocketDownloadHandler::onAboutUploadProgress(int id, double bytesTotal, double bytesSent)
{
	const RequesterSharedPtr requester = requesterById(id);

	if (!requester)
	{
		return;
	}

	const std::shared_ptr<UploadProgressResponse> uploadProgressResponse =
		std::make_shared<UploadProgressResponse>();

	uploadProgressResponse->bytesSent = static_cast<quint64>(bytesSent);
	uploadProgressResponse->bytesTotal = static_cast<quint64>(bytesTotal);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, uploadProgressResponse);
}

void MultiSocketDownloadHandler::onUrlLoaded(int id,
	const QByteArray& url,
	const QByteArray& data,
	const ResponseHeaders& responseHeaders,
	Common::StatusCode statusCode,
	int timeElapsed)
{
	DEBUGLOG << "Loaded: " << url;

	if (statusCode == Common::StatusCode::ProxyAuthenticationRequired407)
	{
		proxyAuthenticationRequired();
	}

	const RequesterSharedPtr requester = requesterByIdAssertIfNotExists(id);

	if (!requester)
	{
		return;
	}

	const DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());
	const DownloadRequestType requestType = request->requestInfo.requestType;

	const bool isRedirectionStatusCode =
		statusCode == Common::StatusCode::MovedPermanently301 ||
		statusCode == Common::StatusCode::MovedTemporarily302;

	const std::shared_ptr<DownloadResponse> response = responseFor(id);

	const Url loadedResourceUrl(url);
	const Url redirectUrlAddress = redirectedUrl(responseHeaders, loadedResourceUrl);

	response->hopsChain.addHop(Hop(loadedResourceUrl, redirectUrlAddress, statusCode, data, responseHeaders, timeElapsed));

	if (isRedirectionStatusCode)
	{
		DEBUGLOG
			<< "Url: "
			<< url
			<< " was redirected to: "
			<< redirectUrlAddress.urlStr();

		if (response->hopsChain.length() == static_cast<size_t>(maxRedirectsToProcess()))
		{
			statusCode = Common::StatusCode::TooManyRedirections;
		}
		else if (isRedirectLoop(response->hopsChain, redirectUrlAddress))
		{
			statusCode = Common::StatusCode::RedirectLoop;

			DEBUGLOG
				<< "redirect loop detected"
				<< url;
		}
		else
		{
			followLocation(request->bodyProcessingCommand, id, redirectUrlAddress, requestType);
			return;
		}
	}

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);

	const int parentId = parentIdFor(id);
	const int parentRequestId = parentId == -1 ? id : parentId;

	m_responses.remove(parentRequestId);
	m_activeRequesters.remove(parentRequestId);

	removeRequestIndexesChain(id);
}


void MultiSocketDownloadHandler::onCurrentParallelTransfersCountChanged(int count)
{
	if (count >= c_maxParallelTransferCount || m_pendingRequesters.isEmpty())
	{
		return;
	}

	RequesterSharedPtr requester = extractFirstUnpausedRequester();

	if (!requester)
	{
		return;
	}

	load(requester);
}

void MultiSocketDownloadHandler::load(RequesterSharedPtr requester)
{
	if (m_multiSocketLoader->currentParallelConnections() >= c_maxParallelTransferCount)
	{
		m_pendingRequesters.enqueue(requester);
		return;
	}

	const DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const int requestId = loadHelper(request->requestInfo, request->bodyProcessingCommand);

	// we add requester to the m_activeRequesters only for the first request
	// redirect requests here will not be stored
	m_activeRequesters[requestId] = requester;
}

int MultiSocketDownloadHandler::loadHelper(const CrawlerRequest& request, DownloadRequest::BodyProcessingCommand bodyProcessingCommand)
{
	int requestId = 0;

	switch (request.requestType)
	{
		case DownloadRequestType::RequestTypeGet:
		{
			requestId = m_multiSocketLoader->get(request.url, bodyProcessingCommand);
			break;
		}
		case DownloadRequestType::RequestTypeHead:
		{
			requestId = m_multiSocketLoader->head(request.url);
			break;
		}
		default:
		{
			ASSERT(!"Unsupported request type");
			break;
		}
	}

	return requestId;
}

//! returns first unpaused requester and also clears expired requesters by searching pass
RequesterSharedPtr MultiSocketDownloadHandler::extractFirstUnpausedRequester()
{
	for (auto first = m_pendingRequesters.begin(); first != m_pendingRequesters.end();)
	{
		RequesterSharedPtr requester = first->lock();

		if (!requester)
		{
			first = m_pendingRequesters.erase(first);
			continue;
		}

		if (m_pausedRequesters.contains(requester.get()))
		{
			++first;
		}
		else
		{
			m_pendingRequesters.erase(first);
			return requester;
		}
	}

	return RequesterSharedPtr();
}

}
