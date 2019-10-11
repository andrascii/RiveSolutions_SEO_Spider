#include "model_controller.h"
#include "unordered_data_collection.h"
#include "page_parser_helpers.h"
#include "crawler_shared_state.h"
#include "finally.h"

namespace
{

using namespace CrawlerEngine;

struct EmitBlocker final
{
	EmitBlocker(UnorderedDataCollection* udc, int turnaround)
		: udc(udc)
		, turnaround(turnaround)
	{
		udc->setPageAddingEmitAbility(false, turnaround);
	}

	~EmitBlocker()
	{
		udc->setPageAddingEmitAbility(true, turnaround);
	}

	UnorderedDataCollection* udc;
	int turnaround;
};

template <typename IterOut, typename IterIn, typename Policy>
void assignIf(IterOut firstOut, IterOut secondOut, IterIn firstIn, IterIn secondIn, Policy&& policy)
{
	DEBUG_ASSERT(std::distance(firstOut, secondOut) == std::distance(firstIn, secondIn));

	for (; firstIn != secondIn; ++firstOut, ++firstIn)
	{
		*firstOut = policy(*firstOut, *firstIn);
	}
}

ParsedPagePtr mergePage(ParsedPagePtr existingPage, ParsedPagePtr incomingParsedPage)
{
	CrawlerEngine::ParsedPage* existingPageRawPtr = existingPage.get();
	CrawlerEngine::ParsedPage* newParsedPage = incomingParsedPage.get();

	if (!existingPage || existingPageRawPtr == newParsedPage)
	{
		return incomingParsedPage;
	}

	incomingParsedPage->linksToThisPage.insert(
		std::begin(incomingParsedPage->linksToThisPage),
		std::begin(existingPage->linksToThisPage),
		std::end(existingPage->linksToThisPage)
	);

	std::vector<bool> storages = existingPage->storages;

	*existingPage = *incomingParsedPage;

// 	const auto policy = [](bool lhs, bool rhs) noexcept
// 	{
// 		return lhs ? true : rhs;
// 	};
//
// 	assignIf(storages.begin(), storages.end(), incomingParsedPage->storages.begin(), incomingParsedPage->storages.end(), policy);
//
	existingPage->storages = storages;

	// we should use the original ParsedPagePtr
	// because there are pages containing links to it
	return existingPage;
}

void checkResourceLinks(ParsedPagePtr page)
{
	QSet<QString> linksToThisPage;
	QSet<QString> linksOnThisPage;

	for (const CrawlerEngine::ResourceLink& link : page->linksToThisPage)
	{
		linksToThisPage.insert(link.resource.lock()->url.toDisplayString());
	}

	for (const CrawlerEngine::ResourceLink& link : page->linksOnThisPage)
	{
		linksOnThisPage.insert(link.resource.lock()->url.toDisplayString());
	}

	ASSERT(page->linksToThisPage.size() == static_cast<size_t>(linksToThisPage.size()));
	ASSERT(page->linksOnThisPage.size() == static_cast<size_t>(linksOnThisPage.size()));
}

bool successfulCode(const ParsedPagePtr& page)
{
	return page->statusCode >= Common::StatusCode::Ok200 &&
		page->statusCode < Common::StatusCode::MultipleChoices300;
}

}

namespace CrawlerEngine
{

ModelController::ModelController()
	: QObject(nullptr)
	, m_data(new UnorderedDataCollection(this))
{
}

const UnorderedDataCollection* ModelController::data() const noexcept
{
	return m_data;
}

UnorderedDataCollection* ModelController::data() noexcept
{
	return m_data;
}

void ModelController::setWebCrawlerOptions(const CrawlerOptionsData& optionsData)
{
	m_crawlerOptionsData = optionsData;
}

void ModelController::clearData()
{
	data()->clearData();

	CrawlerSharedState::instance()->setModelControllerAcceptedLinksCount(0);
	CrawlerSharedState::instance()->setModelControllerCrawledLinksCount(0);
}

void ModelController::preparePageForRefresh(ParsedPage* parsedPage, int turnaround)
{
	const auto fakeDeleter = [](ParsedPage*) noexcept {};
	ParsedPagePtr pointer(parsedPage, fakeDeleter);

	data()->prepareCollectionForRefreshPage(pointer, turnaround);
}

void ModelController::handleWorkerResult(WorkerResult workerResult) noexcept
{
	if (workerResult.turnaround() != CrawlerSharedState::instance()->turnaround())
	{
		return;
	}

	EmitBlocker emitBlocker(data(), workerResult.turnaround());
	Common::Finally incrementCrawledLinksCountGuard([] { CrawlerSharedState::instance()->incrementModelControllerCrawledLinksCount(); });
	Common::Finally clearAllResourcesOnPageGuard([workerResult] { workerResult.incomingPageConstRef()->allResourcesOnPage.clear(); });

	const auto refreshDoneEmit = [&]
	{
		if (workerResult.isRefreshResult())
		{
			emit refreshPageDone();
		}
	};

	Common::Finally finallyObject(refreshDoneEmit);

	ASSERT(workerResult.incomingPage()->resourceType >= ResourceType::ResourceHtml &&
		workerResult.incomingPage()->resourceType <= ResourceType::ResourceOther);

	bool secondGetRequest = false;

	if (!workerResult.isRefreshResult())
	{
		const ParsedPagePtr existingPage = data()->parsedPage(workerResult.incomingPage(), StorageType::CrawledUrlStorageType);

		if (existingPage && workerResult.requestType() == DownloadRequestType::RequestTypeHead)
		{
			return;
		}
		else if(existingPage)
		{
			secondGetRequest = true;
		}

	#ifdef QT_DEBUG
		DEBUG_ASSERT(!secondGetRequest || existingPage->headRequest);
		workerResult.incomingPage()->headRequest = workerResult.requestType() == DownloadRequestType::RequestTypeHead;
		DEBUG_ASSERT(!workerResult.incomingPage()->headRequest || workerResult.incomingPage()->wordCount == 0);
	#endif

		workerResult.incomingPage() = mergePage(existingPage, workerResult.incomingPage());
	}
	else if (workerResult.storagesBeforeRemoving()[StorageType::NofollowLinksStorageType])
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::NofollowLinksStorageType, workerResult.turnaround());
	}

	fixParsedPageResourceType(workerResult.incomingPage());

	if (!resourceShouldBeProcessed(workerResult.incomingPage()->resourceType, m_crawlerOptionsData))
	{
		removeResourceFromPendingStorageIfNeeded(workerResult.incomingPage(), workerResult.turnaround());
		return;
	}

	processParsedPageHtmlResources(workerResult, secondGetRequest);
	processParsedPageResources(workerResult, secondGetRequest);

	processParsedPageStatusCode(workerResult, secondGetRequest);
	processParsedPageUrl(workerResult, secondGetRequest);

	if (workerResult.incomingPage()->resourceType == ResourceType::ResourceHtml)
	{
		processParsedPageTitle(workerResult, secondGetRequest);
		processParsedPageMetaDescription(workerResult, secondGetRequest);
		processParsedPageMetaKeywords(workerResult, secondGetRequest);
		processParsedPageH1(workerResult, secondGetRequest);
		processParsedPageH2(workerResult, secondGetRequest);
		handlePresenceYandexMetricaCounters(workerResult, secondGetRequest);
		handlePresenceGoogleAnalyticsCounters(workerResult, secondGetRequest);
	}
	else if (workerResult.incomingPage()->resourceType == ResourceType::ResourceImage)
	{
		processParsedPageImage(workerResult, false, secondGetRequest);
	}

	data()->removeParsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType, workerResult.turnaround());

	if (!m_linksToPageChanges.changes.empty())
	{
		data()->parsedPageLinksToThisResourceChanged(m_linksToPageChanges, workerResult.turnaround());
		m_linksToPageChanges.changes.clear();
	}

	DEBUGLOG << "crawled" << workerResult.incomingPage()->url.urlStr();

	if (!workerResult.isRefreshResult())
	{
		//DEBUG_ASSERT(!workerResult.incomingPage()->linksToThisPage.empty() ||
		//	data()->size(StorageType::CrawledUrlStorageType) == 1);

#ifdef QT_DEBUG
		const bool redirect = workerResult.incomingPage()->redirectedUrl.isValid();
		const bool external = workerResult.incomingPage()->isThisExternalPage;
		if (redirect && external)
		{
			const bool hasLinksOnThispage =
				workerResult.incomingPage()->linksOnThisPage.size() == 1 &&
				!workerResult.incomingPage()->linksOnThisPage.front().resource.expired();

			if (!hasLinksOnThispage)
			{
				INFOLOG << "Invalid links on this page for"
					<< workerResult.incomingPage()->redirectedUrl.toDisplayString()
					<< " resources on page:"
					<< workerResult.incomingPage()->allResourcesOnPage.size();
				DEBUG_ASSERT(false);
			}
		}
#endif
	}
}

void ModelController::processParsedPageUrl(WorkerResult& workerResult, bool secondGetRequest)
{
	if (secondGetRequest)
	{
		return;
	}

	const Url url = workerResult.incomingPage()->url;
	const QString urlStr = url.toString();

	if (workerResult.isRefreshResult())
	{
		const auto[oldPage, storageType] = tryGetPageFromCrawledOrPendingStorage(workerResult.incomingPage());

		ASSERT(oldPage);

		data()->replaceParsedPage(oldPage, workerResult.incomingPage(), storageType, workerResult.turnaround());
	}
	else
	{
		data()->addParsedPage(workerResult, StorageType::CrawledUrlStorageType);
	}

	Common::Finally incrementAcceptedLinksCountGuard([] { CrawlerSharedState::instance()->incrementModelControllerAcceptedLinksCount(); });

	calculatePageLevel(workerResult.incomingPage());

	if (url.host() != m_crawlerOptionsData.startCrawlingPage.host())
	{
		data()->addParsedPage(workerResult, StorageType::ExternalUrlStorageType);
	}

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	if (urlStr.toLower() != urlStr)
	{
		data()->addParsedPage(workerResult, StorageType::UpperCaseUrlStorageType);
	}

	if (workerResult.incomingPage()->linksOnThisPage.size() > static_cast<size_t>(m_crawlerOptionsData.maxLinksCountOnPage))
	{
		data()->addParsedPage(workerResult, StorageType::TooManyLinksOnPageStorageType);
	}

	if (workerResult.incomingPage()->hasMetaRefreshTag)
	{
		data()->addParsedPage(workerResult, StorageType::ContainsMetaRefreshTagStorageType);
	}

	if (workerResult.incomingPage()->hasFrames)
	{
		data()->addParsedPage(workerResult, StorageType::ContainsFramesStorageType);
	}

	bool hasNonAscii = false;

	for (int i = 0; i < urlStr.size(); ++i)
	{
		if (urlStr.at(i).unicode() > 127)
		{
			hasNonAscii = true;
			break;
		}
	}

	if (hasNonAscii)
	{
		data()->addParsedPage(workerResult, StorageType::NonAsciiCharacterUrlStorageType);
	}

	if (workerResult.incomingPage()->redirectedUrl.isValid())
	{
		const QString host = workerResult.incomingPage()->url.host().toLower();
		const QString redirectedHost = workerResult.incomingPage()->redirectedUrl.host().toLower();

		if (host != redirectedHost)
		{
			const QString www = QString("www.");
			if (www + host == redirectedHost || www + redirectedHost == host)
			{
				data()->addParsedPage(workerResult, StorageType::WwwRedirectionsUrlStorageType);
			}
		}
	}
}

void ModelController::processParsedPageTitle(WorkerResult& workerResult, bool secondGetRequest)
{
	Q_UNUSED(secondGetRequest);

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	const Common::StatusCode responseCode = workerResult.incomingPage()->statusCode;
	const QString title = workerResult.incomingPage()->title;
	const QString h1 = workerResult.incomingPage()->firstH1;

	const bool successfulResponseCode = successfulCode(workerResult.incomingPage());

	if ((title.isEmpty() && successfulResponseCode))
	{
		data()->addParsedPage(workerResult, StorageType::EmptyTitleUrlStorageType);
	}
	else if (title.size() > m_crawlerOptionsData.maxTitleLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongTitleUrlStorageType);
	}
	else if (title.size() < m_crawlerOptionsData.minTitleLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooShortTitleUrlStorageType);
	}

	if (!title.isEmpty() && successfulResponseCode)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllTitlesUrlStorageType,
			StorageType::DuplicatedTitleUrlStorageType, workerResult.turnaround());

		data()->addParsedPage(workerResult, StorageType::AllTitlesUrlStorageType);
	}

	if (!h1.isEmpty() && h1 == title && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::DuplicatedH1TitleUrlStorageType);
	}

	if (workerResult.incomingPage()->hasSeveralTitleTags && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::SeveralTitleUrlStorageType);
	}
}

void ModelController::processParsedPageMetaDescription(WorkerResult& workerResult, bool secondGetRequest)
{
	Q_UNUSED(secondGetRequest);

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(workerResult.incomingPage());

	const int metaDescriptionLength = workerResult.incomingPage()->metaDescription.size();

	if (metaDescriptionLength == 0 && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::EmptyMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength > m_crawlerOptionsData.maxDescriptionLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < m_crawlerOptionsData.minDescriptionLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0 && successfulResponseCode)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllMetaDescriptionsUrlStorageType,
			StorageType::DuplicatedMetaDescriptionUrlStorageType, workerResult.turnaround());

		data()->addParsedPage(workerResult, StorageType::AllMetaDescriptionsUrlStorageType);
	}

	if (workerResult.incomingPage()->hasSeveralMetaDescriptionTags && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::SeveralMetaDescriptionUrlStorageType);
	}
}

void ModelController::processParsedPageMetaKeywords(WorkerResult& workerResult, bool secondGetRequest)
{
	Q_UNUSED(secondGetRequest)

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(workerResult.incomingPage());

	const int metaKeywordsLength = workerResult.incomingPage()->metaKeywords.size();

	if (metaKeywordsLength == 0 && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::EmptyMetaKeywordsUrlStorageType);
	}

	if (metaKeywordsLength > 0 && successfulResponseCode)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllMetaKeywordsUrlStorageType,
			StorageType::DuplicatedMetaKeywordsUrlStorageType, workerResult.turnaround());

		data()->addParsedPage(workerResult, StorageType::AllMetaKeywordsUrlStorageType);
	}


	if (workerResult.incomingPage()->hasSeveralMetaKeywordsTags && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::SeveralMetaKeywordsUrlStorageType);
	}
}

void ModelController::processParsedPageH1(WorkerResult& workerResult, bool secondGetRequest)
{
	Q_UNUSED(secondGetRequest);

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(workerResult.incomingPage());

	if (!successfulResponseCode)
	{
		return;
	}

	const int h1Length = workerResult.incomingPage()->firstH1.size();

	if (!h1Length)
	{
		data()->addParsedPage(workerResult, StorageType::MissingH1UrlStorageType);
	}
	else if (h1Length > m_crawlerOptionsData.maxH1LengthChars)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongH1UrlStorageType);
	}

	if (h1Length > 0)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllH1UrlStorageType,
			StorageType::DuplicatedH1UrlStorageType, workerResult.turnaround());

		data()->addParsedPage(workerResult, StorageType::AllH1UrlStorageType);
	}

	if (workerResult.incomingPage()->hasSeveralH1Tags)
	{
		data()->addParsedPage(workerResult, StorageType::SeveralH1UrlStorageType);
	}
}

void ModelController::processParsedPageH2(WorkerResult& workerResult, bool secondGetRequest)
{
	Q_UNUSED(secondGetRequest);

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(workerResult.incomingPage());

	if (!successfulResponseCode)
	{
		return;
	}

	const int h2Length = workerResult.incomingPage()->firstH2.size();

	if (!h2Length)
	{
		data()->addParsedPage(workerResult, StorageType::MissingH2UrlStorageType);
	}
	else if (h2Length > m_crawlerOptionsData.maxH2LengthChars)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongH2UrlStorageType);
	}

	if (h2Length > 0)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllH2UrlStorageType,
			StorageType::DuplicatedH2UrlStorageType, workerResult.turnaround());

		data()->addParsedPage(workerResult, StorageType::AllH2UrlStorageType);
	}

	if (workerResult.incomingPage()->hasSeveralEqualH2Tags)
	{
		data()->addParsedPage(workerResult, StorageType::SeveralH2UrlStorageType);
	}
}

void ModelController::processParsedPageImage(WorkerResult& workerResult, bool checkOnlyLastResource, bool secondGetRequest)
{
	Q_UNUSED(secondGetRequest);

	class CounterIncrementor final
	{
	public:
		CounterIncrementor(std::size_t& ref)
			: m_ref(ref)
		{
		}

		~CounterIncrementor()
		{
			++m_ref;
		}

	private:
		std::size_t& m_ref;
	};

	const int sizeKB = workerResult.incomingPage()->pageSizeKilobytes;

	if (sizeKB > m_crawlerOptionsData.maxImageSizeKb &&
		!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::TooBigImageStorageType))
	{
		data()->addParsedPage(workerResult, StorageType::TooBigImageStorageType);
	}

	if (workerResult.incomingPage()->statusCode >= Common::StatusCode::BadRequest400 &&
		workerResult.incomingPage()->statusCode < Common::StatusCode::BlockedByRobotsTxt &&
		!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::BrokenImagesStorageType))
	{
		data()->addParsedPage(workerResult, StorageType::BrokenImagesStorageType);
	}

	std::size_t index = 0;
	const std::size_t lastIndex = workerResult.incomingPage()->linksToThisPage.size() - 1;

	for (const ResourceLink& linkToThisImage : workerResult.incomingPage()->linksToThisPage)
	{
		CounterIncrementor incrementor(index);

		if (checkOnlyLastResource && index != lastIndex)
		{
			continue;
		}

		if (linkToThisImage.resourceSource == ResourceSource::SourceTagImg)
		{
			const int altLength = linkToThisImage.altOrTitle.size();

			if (altLength > m_crawlerOptionsData.maxImageAltTextChars &&
				!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::TooLongAltTextImageStorageType))
			{
				data()->addParsedPage(workerResult, StorageType::TooLongAltTextImageStorageType);
			}

			if (altLength == 0 && !data()->isParsedPageExists(workerResult.incomingPage(), StorageType::MissingAltTextImageStorageType))
			{
				data()->addParsedPage(workerResult, StorageType::MissingAltTextImageStorageType);
			}
		}
	}
}

void ModelController::processParsedPageStatusCode(WorkerResult& workerResult, bool secondGetRequest)
{
	if (secondGetRequest)
	{
		return;
	}

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::NotFound404 ||
		workerResult.incomingPage()->statusCode == Common::StatusCode::UnknownNetworkError)
	{
		data()->addParsedPage(workerResult, StorageType::BrokenLinks);
	}

	if (workerResult.incomingPage()->statusCode >= Common::StatusCode::BadRequest400 &&
		workerResult.incomingPage()->statusCode <= Common::StatusCode::UnsupportedMediaType415)
	{
		data()->addParsedPage(workerResult, StorageType::Status4xxStorageType);
	}

	if (workerResult.incomingPage()->statusCode >= Common::StatusCode::InternalServerError500 &&
		workerResult.incomingPage()->statusCode <= Common::StatusCode::HttpVersionNotSupported505)
	{
		data()->addParsedPage(workerResult, StorageType::Status5xxStorageType);
	}

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::MovedPermanently301)
	{
		data()->addParsedPage(workerResult, StorageType::Status301StorageType);
	}

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::MovedTemporarily302)
	{
		data()->addParsedPage(workerResult, StorageType::Status302StorageType);
	}

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::TooManyRedirections)
	{
		data()->addParsedPage(workerResult, StorageType::TooManyRedirectsStorageType);
	}

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::Timeout)
	{
		data()->addParsedPage(workerResult, StorageType::TimeoutStorageType);
	}

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::TooLongLink)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongUrlStorageType);
	}
}

void ModelController::processParsedPageHtmlResources(WorkerResult& workerResult, bool secondGetRequest)
{
	if (workerResult.incomingPage()->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	const ParsedPagePtr pendingPage = data()->parsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);
	workerResult.incomingPage() = mergePage(pendingPage, workerResult.incomingPage());

	addToBlockedByXrobotsTagIfNeeded(workerResult);

	const StorageType storage = workerResult.incomingPage()->isThisExternalPage ?
		StorageType::ExternalHtmlResourcesStorageType : StorageType::HtmlResourcesStorageType;

	if (!secondGetRequest)
	{
		data()->addParsedPage(workerResult, storage);
		if (storage == StorageType::HtmlResourcesStorageType)
		{
			processDataExtractions(workerResult);
		}
	}

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		const bool doFollow = std::any_of(workerResult.incomingPage()->linksToThisPage.begin(), workerResult.incomingPage()->linksToThisPage.end(),
			[](const ResourceLink& link) { return link.linkParameter == LinkParameter::DofollowParameter; });

		if (doFollow && !secondGetRequest)
		{
			// TODO: we can add a "do follow" link later, so we should take this into account
			DEBUG_ASSERT(!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::ExternalDoFollowUrlResourcesStorageType));
			data()->addParsedPage(workerResult, StorageType::ExternalDoFollowUrlResourcesStorageType);
		}

		DEBUG_ASSERT(!workerResult.incomingPage()->redirectedUrl.isValid() ||
			workerResult.incomingPage()->allResourcesOnPage.size() == 1);

		if (!workerResult.incomingPage()->redirectedUrl.isValid())
		{
			// do not parse resources from an external one
			return;
		}
	}

	if (workerResult.incomingPage()->canonicalUrl.isValid() && !workerResult.incomingPage()->isThisExternalPage)
	{
		addDuplicates(workerResult.incomingPage(),
			StorageType::AllCanonicalUrlResourcesStorageType,
				StorageType::DuplicatedCanonicalUrlResourcesStorageType, workerResult.turnaround(), false);

		data()->addParsedPage(workerResult, StorageType::AllCanonicalUrlResourcesStorageType);

		if (!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::UniqueCanonicalUrlResourcesStorageType))
		{
			data()->addParsedPage(workerResult, StorageType::UniqueCanonicalUrlResourcesStorageType);
		}
	}

	if (workerResult.incomingPage()->pageSizeKilobytes > m_crawlerOptionsData.maxPageSizeKb)
	{
		data()->addParsedPage(workerResult, StorageType::TooBigHtmlResourcesStorageType);
	}

	for (const ResourceOnPage& resource : workerResult.incomingPage()->allResourcesOnPage)
	{
		if (resource.resourceType != ResourceType::ResourceHtml)
		{
			continue;
		}

		if (workerResult.incomingPage()->isThisExternalPage &&
			resource.link.resourceSource != ResourceSource::SourceRedirectUrl)
		{
			continue;
		}

		ParsedPagePtr resourcePage = parsedPageFromResource(resource);

		const bool loadedPageIsResource = resourcePage->url.compare(workerResult.incomingPage()->url);

		ParsedPagePtr existingResource = loadedPageIsResource ?
			workerResult.incomingPage() :
			takeFromCrawledOrPendingStorage(resourcePage);


		if (existingResource)
		{
			setLinksForResourcePageAndLoadedPage(existingResource, workerResult, resource);

			m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ existingResource, existingResource->linksToThisPage.size() - 1 });

			// add it only if resource url is not equal to parent page url
			if (!workerResult.incomingPage()->url.compare(existingResource->url))
			{
				addIndexingBlockingPage(existingResource, true, resource, workerResult.incomingPage()->isBlockedByMetaRobots, workerResult.turnaround());
			}
		}
		else
		{
			ParsedPagePtr pendingResource = parsedPageFromResource(resource);
			setLinksForResourcePageAndLoadedPage(pendingResource, workerResult, resource);

			// we should always add resource in pending links, even if it's blocked for now
			// because it can be not blocked from other pages and we will need in this case merge results
			// otherwise, we'll loose some links to this page

			// add it only if resource url is not equal to parent page url
			if (!workerResult.incomingPage()->url.compare(pendingResource->url))
			{
				addIndexingBlockingPage(pendingResource, false, resource, workerResult.incomingPage()->isBlockedByMetaRobots, workerResult.turnaround());
			}

			data()->addParsedPage(pendingResource, StorageType::PendingResourcesStorageType, workerResult.turnaround());

			DEBUG_ASSERT(data()->isParsedPageExists(pendingResource, StorageType::PendingResourcesStorageType));
		}
	}
}

void ModelController::processDataExtractions(WorkerResult& workerResult)
{
	const QMap<int, QString>& extractions = workerResult.incomingPage()->dataExtractions;
	if (!extractions.isEmpty())
	{
		data()->addParsedPage(workerResult, StorageType::DataExtractionStorageType);
	}
}

void ModelController::processParsedPageResources(WorkerResult& workerResult, bool secondGetRequest)
{
	static const QMap<ResourceType, StorageType> s_storageTypes
	{
		{ ResourceType::ResourceImage, StorageType::ImageResourcesStorageType },
		{ ResourceType::ResourceJavaScript, StorageType::JavaScriptResourcesStorageType },
		{ ResourceType::ResourceStyleSheet, StorageType::StyleSheetResourcesStorageType },
		{ ResourceType::ResourceFlash, StorageType::FlashResourcesStorageType },
		{ ResourceType::ResourceVideo, StorageType::VideoResourcesStorageType },
		{ ResourceType::ResourceOther, StorageType::OtherResourcesStorageType },
	};

	static const QMap<ResourceType, StorageType> s_externalStorageTypes
	{
		{ ResourceType::ResourceImage, StorageType::ExternalImageResourcesStorageType },
		{ ResourceType::ResourceJavaScript, StorageType::ExternalJavaScriptResourcesStorageType },
		{ ResourceType::ResourceStyleSheet, StorageType::ExternalStyleSheetResourcesStorageType },
		{ ResourceType::ResourceFlash, StorageType::ExternalFlashResourcesStorageType },
		{ ResourceType::ResourceVideo, StorageType::ExternalVideoResourcesStorageType },
		{ ResourceType::ResourceOther, StorageType::ExternalOtherResourcesStorageType },
	};

	const bool http = PageParserHelpers::isHttpOrHttpsScheme(workerResult.incomingPage()->url);
	const bool externalOrNotHttp = workerResult.incomingPage()->isThisExternalPage || !http;

	if (workerResult.incomingPage()->resourceType != ResourceType::ResourceHtml && !secondGetRequest)
	{
		const ParsedPagePtr pendingPageRaw = data()->parsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);
		workerResult.incomingPage() = mergePage(pendingPageRaw, workerResult.incomingPage());

		const ResourceType resourceType = workerResult.incomingPage()->resourceType;

		const StorageType storage = externalOrNotHttp
			? s_externalStorageTypes[resourceType]
			: s_storageTypes[resourceType];

		data()->addParsedPage(workerResult, storage);
	}

	if (workerResult.incomingPage()->isThisExternalPage && !workerResult.incomingPage()->redirectedUrl.isValid())
	{
		return;
	}

	for (const ResourceOnPage& resource : workerResult.incomingPage()->allResourcesOnPage)
	{
		const QString resourceDisplayUrl = resource.link.url.toDisplayString();
		const ResourceType resourceType = resource.resourceType;
		const ResourceSource resourceSource = resource.link.resourceSource;

		ParsedPagePtr temporaryResource = parsedPageFromResource(resource);
		ParsedPagePtr resourcePage = data()->parsedPage(temporaryResource, StorageType::CrawledUrlStorageType);

		ResourceLink linkToThisResource
		{
			workerResult.incomingPage(),
			workerResult.incomingPage()->url,
			resource.link.linkParameter,
			resourceSource,
			resource.link.altOrTitle
		};

		if (resourceType == ResourceType::ResourceHtml &&
			resourceSource == ResourceSource::SourceRedirectUrl &&
			!resourcePage)
		{
			/// We process the resources for which the redirect occurred.
			/// Just put them in pending without assignment the resource.
			/// The resource type will be set from the Content-Type when the resource is finally loaded.

			if (data()->parsedPage(temporaryResource, StorageType::PendingResourcesStorageType))
			{
				/// this resource already had been added to the downloaded page
				continue;
			}

			workerResult.incomingPage()->linksOnThisPage.emplace_back(ResourceLink{ temporaryResource, temporaryResource->url,
				resource.link.linkParameter, resourceSource, resource.link.altOrTitle });

			temporaryResource->linksToThisPage.emplace_back(linkToThisResource);

			data()->addParsedPage(temporaryResource, StorageType::PendingResourcesStorageType, workerResult.turnaround());
		}

		if (resourceType == ResourceType::ResourceHtml)
		{
			continue;
		}

		if ((!resourceShouldBeProcessed(resourceType, m_crawlerOptionsData) &&
			resourceSource != ResourceSource::SourceRedirectUrl) ||
			resourceDisplayUrl.startsWith("javascript:") ||
			resourceDisplayUrl.startsWith("#"))
		{
			continue;
		}

		if (workerResult.incomingPage()->isThisExternalPage &&
			resourceSource != ResourceSource::SourceRedirectUrl)
		{
			continue;
		}

		const bool httpResource = PageParserHelpers::isHttpOrHttpsScheme(resource.link.url);

		const bool externalOrNotHttpResource = !httpResource ||
			PageParserHelpers::isUrlExternal(workerResult.incomingPage()->url, temporaryResource->url, m_crawlerOptionsData.checkSubdomains);

		const StorageType storage = externalOrNotHttpResource ?
			s_externalStorageTypes[resourceType] :
			s_storageTypes[resourceType];

		ParsedPagePtr newOrExistingResource =
			temporaryResource->url.compare(workerResult.incomingPage()->url) ?
			workerResult.incomingPage() : resourcePage;

		if (!newOrExistingResource)
		{
			ParsedPagePtr newOrExistingResource = data()->parsedPage(temporaryResource, storage);
		}

		const bool existingImageResource = newOrExistingResource &&
			newOrExistingResource->resourceType == ResourceType::ResourceImage &&
			resourceSource == ResourceSource::SourceTagImg;

		if (!newOrExistingResource)
		{
			newOrExistingResource = data()->parsedPage(temporaryResource, StorageType::PendingResourcesStorageType);
		}

		if (!newOrExistingResource)
		{
			newOrExistingResource = temporaryResource;

			if (httpResource || resourceSource == ResourceSource::SourceRedirectUrl)
			{
				// always add all resources in pending storage even if they're blocked
				// because every blocked resource can be removed in future from these lists
				// and we should not loose links to/from this page
				data()->addParsedPage(newOrExistingResource, StorageType::PendingResourcesStorageType, workerResult.turnaround());
			}
			else if (!httpResource && !data()->isParsedPageExists(newOrExistingResource, StorageType::AllOtherResourcesStorageType))
			{
				data()->addParsedPage(newOrExistingResource, StorageType::AllOtherResourcesStorageType, workerResult.turnaround());
				data()->addParsedPage(newOrExistingResource, storage, workerResult.turnaround());
			}
		}

		workerResult.incomingPage()->linksOnThisPage.emplace_back(ResourceLink { newOrExistingResource, newOrExistingResource->url,
			resource.link.linkParameter, resourceSource, resource.link.altOrTitle });

		newOrExistingResource->linksToThisPage.emplace_back(linkToThisResource);

		m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ newOrExistingResource, newOrExistingResource->linksToThisPage.size() - 1 });

		newOrExistingResource->resourceType = resourceType;

		// special case: parse image resource again because it can have now empty or too short/long alt text
		if (existingImageResource)
		{
			WorkerResult result(newOrExistingResource,
				workerResult.turnaround(),
				workerResult.isRefreshResult(),
				workerResult.requestType(),
				workerResult.storagesBeforeRemoving());

			processParsedPageImage(result, true, secondGetRequest);
		}
	}
}

void ModelController::fixParsedPageResourceType(ParsedPagePtr& incomingPage) const noexcept
{
	ParsedPagePtr pendingOrCrawled = takeFromCrawledOrPendingStorage(incomingPage);

	if (pendingOrCrawled && pendingOrCrawled->resourceType != ResourceType::ResourceHtml)
	{
		incomingPage->resourceType = pendingOrCrawled->resourceType;
/*
		if (pendingOrCrawled->linksToThisPage.size() == 1 &&
			pendingOrCrawled->linksToThisPage.begin()->resourceSource == ResourceSource::SourceRedirectUrl &&
			pendingOrCrawled->linksToThisPage.begin()->resource.expired() &&
			pendingOrCrawled->linksToThisPage.begin()->resource.lock()->redirectedUrl.isValid())
		{
			incomingPage->resourceType = pendingOrCrawled->linksToThisPage.begin()->resource.lock()->resourceType;
		}*/
	}
}

bool ModelController::resourceShouldBeProcessed(ResourceType resourceType, const CrawlerOptionsData& optionsData) noexcept
{
	switch (resourceType)
	{
		case ResourceType::ResourceHtml:
		{
			return true;
		}
		case ResourceType::ResourceImage:
		{
			return optionsData.parserTypeFlags.testFlag(ImagesResourcesParserType);
		}
		case ResourceType::ResourceJavaScript:
		{
			return optionsData.parserTypeFlags.testFlag(JavaScriptResourcesParserType);
		}
		case ResourceType::ResourceStyleSheet:
		{
			return optionsData.parserTypeFlags.testFlag(CssResourcesParserType);
		}
		case ResourceType::ResourceFlash:
		{
			return optionsData.parserTypeFlags.testFlag(FlashResourcesParserType);
		}
		case ResourceType::ResourceVideo:
		{
			return optionsData.parserTypeFlags.testFlag(VideoResourcesParserType);
		}
		case ResourceType::ResourceOther:
		{
			return optionsData.parserTypeFlags.testFlag(OtherResourcesParserType);
		}
		default:
		{
			ASSERT(!"Unexpected resource type");
			break;
		}
	}

	return false;
}

void ModelController::removeResourceFromPendingStorageIfNeeded(ParsedPagePtr& incomingPage, int turnaround) noexcept
{
	const ParsedPagePtr pendingPage = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
	if (!pendingPage)
	{
		return;
	}

	const std::vector<bool>& storages = pendingPage->storages;
	bool shouldBeRemovedFromPendingStorage = true;
	const int size = qMin(static_cast<int>(StorageType::EndEnumStorageType), static_cast<int>(storages.size()));
	for (int i = 0; i < size; ++i)
	{
		if (i != StorageType::PendingResourcesStorageType && storages[i])
		{
			shouldBeRemovedFromPendingStorage = false;
			break;
		}
	}
	if (shouldBeRemovedFromPendingStorage)
	{
		data()->removeParsedPage(pendingPage, StorageType::PendingResourcesStorageType, turnaround);
	}
}

void ModelController::calculatePageLevel(ParsedPagePtr& incomingPage) const noexcept
{
	int level = 1;
	bool hasParentResources = false;
	for (const ResourceLink& link : incomingPage->linksToThisPage)
	{
		if (link.resource.expired() || link.url.compare(incomingPage->url))
		{
			continue;
		}

		ParsedPagePtr parent = link.resource.lock();

		if (!hasParentResources)
		{
			hasParentResources = true;
			level = invalidPageLevel;
		}

		if (parent->pageLevel + 1 < level)
		{
			level = parent->pageLevel == 1 && parent->url.compare(m_crawlerOptionsData.startCrawlingPage)
				? 1 : parent->pageLevel + 1;
		}
	}


	const bool updateChildren = incomingPage->pageLevel > level;

	//ASSERT(level != invalidPageLevel);

	incomingPage->pageLevel = level;

	if (!updateChildren)
	{
		return;
	}

	for (const ResourceLink& link : incomingPage->linksOnThisPage)
	{
		if (link.resource.expired())
		{
			continue;
		}

		ParsedPagePtr child = link.resource.lock();
		setPageLevel(child, level + 1);
	}
}

void ModelController::setPageLevel(ParsedPagePtr& page, int level) const noexcept
{
	if (page->pageLevel <= level)
	{
		return;
	}

	page->pageLevel = level;

	for (const ResourceLink& link : page->linksOnThisPage)
	{
		if (link.resource.expired())
		{
			continue;
		}

		ParsedPagePtr child = link.resource.lock();
		setPageLevel(child, level + 1);
	}
}

void ModelController::addDuplicates(ParsedPagePtr& incomingPage, StorageType lookupStorage, StorageType destStorage, int turnaround, bool checkCanonicals)
{
	if (data()->isParsedPageExists(incomingPage, destStorage))
	{
		data()->addParsedPage(incomingPage, destStorage, turnaround);
		return;
	}

	const auto predicate = [&page = incomingPage, checkCanonicals](const ParsedPagePtr& candidatePage)
	{
		return
			// discard pages that are different only by trailing slash
			candidatePage->url.canonizedUrlStr() !=
			page->url.canonizedUrlStr() &&

			// and discard pages with the same canonical url
			(!checkCanonicals || candidatePage->canonicalUrl.canonizedUrlStr().isEmpty() ||
			candidatePage->canonicalUrl.canonizedUrlStr() != page->canonicalUrl.canonizedUrlStr());
	};

	const auto alwaysTruePredicate = [](const ParsedPagePtr&) { return true; };

	const std::vector<ParsedPagePtr> duplicatesWithDifferentCanonical =
		data()->allParsedPages(incomingPage, lookupStorage, predicate);

	if (!duplicatesWithDifferentCanonical.empty())
	{
		std::vector<ParsedPagePtr> allDuplicates =
			data()->allParsedPages(incomingPage, lookupStorage, alwaysTruePredicate);

		for (ParsedPagePtr& duplicate : allDuplicates)
		{
			DEBUG_ASSERT(!duplicate->storages[static_cast<std::size_t>(destStorage)]);

			data()->addParsedPage(duplicate, destStorage, turnaround);
		}

		data()->addParsedPage(incomingPage, destStorage, turnaround);
	}
}

ParsedPagePtr ModelController::parsedPageFromResource(const ResourceOnPage& resource) const
{
	ParsedPagePtr parsedPage = std::make_shared<ParsedPage>();
	parsedPage->url = resource.link.url;

	return parsedPage;
}

// #define CHECK_IS_RESOURCE_EXIST_CONSISTENCY
bool isResourceIxist(UnorderedDataCollection* collection, ParsedPagePtr& resource, StorageType storage, bool existingResource)
{
	// BEWARE: if existingResource is true - use this function in cases when it is really existing resource
	// Existing resource means that resource (not incoming page!) in pending or crawled pages (this resource was added/parsed from another page before)

	// If you're trying to use it with an incoming page, this function can be executed incorrectly because this page can be added
	// in some storages (crawled or pending), but not added in the chekced storages yet.

	// uncomment #define CHECK_IS_RESOURCE_EXIST_CONSISTENCY to check if this function is executed correctly

	// if UnorderedDataCollection::isParsedPageExists will be optimized, remove this function

	DEBUG_ASSERT(!existingResource ||
		resource->storages.size() > StorageType::PendingResourcesStorageType && resource->storages[StorageType::PendingResourcesStorageType] ||
		resource->storages.size() > StorageType::CrawledUrlStorageType && resource->storages[StorageType::CrawledUrlStorageType]);

	if (existingResource)
	{
		const bool result = resource->storages.size() > static_cast<size_t>(storage) && resource->storages[storage];
#ifdef CHECK_IS_RESOURCE_EXIST_CONSISTENCY
		DEBUG_ASSERT(collection->isParsedPageExists(resource, storage) == result);
#endif
		return result;
	}

	return collection->isParsedPageExists(resource, storage);
}

QSet<StorageType> ModelController::addIndexingBlockingPage(ParsedPagePtr& pageFromResource, bool existingResource, const ResourceOnPage& resource, bool isParentResourceBlockedByMetaRobots, int turnaround)
{
	Q_UNUSED(existingResource);
	const bool isBlockedByMetaRobots = pageFromResource->isBlockedByMetaRobots;
	DEBUG_ASSERT(!isBlockedByMetaRobots || existingResource);

	const bool isBlockedByXRobotsTag = isParentResourceBlockedByMetaRobots || isBlockedByMetaRobots;
	const bool isAllowedByXRobotsTag = !isBlockedByXRobotsTag;
	const bool isBlockedByRobotsTxt = resource.restrictions.testFlag(Restriction::RestrictionBlockedByRobotsTxtRules);
	const bool isNofollowResource = resource.link.linkParameter == LinkParameter::NofollowParameter;
	const bool isDofollowResource = !isNofollowResource;

	const bool resourceInBlockedByRobotsTxtStorage = isResourceIxist(data(), pageFromResource, StorageType::BlockedByRobotsTxtStorageType, existingResource);
	const bool resourceInBlockedByXRobotsTagStorage = isResourceIxist(data(), pageFromResource, StorageType::BlockedByXRobotsTagStorageType, existingResource);
	const bool resourceInAllowedByXRobotsTagStorage = isResourceIxist(data(), pageFromResource, StorageType::AllowedByXRobotsTagStorageType, existingResource);
	const bool resourceInNoFollowLinksStorage = isResourceIxist(data(), pageFromResource, StorageType::NofollowLinksStorageType, existingResource);
	const bool resourceInDoFollowLinksStorage = isResourceIxist(data(), pageFromResource, StorageType::DofollowUrlStorageType, existingResource);
	const bool resourceInBlockedForSEIndexStorage = isResourceIxist(data(), pageFromResource, StorageType::BlockedForSEIndexingStorageType, existingResource);

	QSet<StorageType> blockingStoragesSet;

	if (isBlockedByRobotsTxt && !resourceInBlockedByRobotsTxtStorage)
	{
		data()->addParsedPage(pageFromResource, StorageType::BlockedByRobotsTxtStorageType, turnaround);
		blockingStoragesSet << StorageType::BlockedByRobotsTxtStorageType;
	}

	if (isBlockedByXRobotsTag && !resourceInBlockedByXRobotsTagStorage && !resourceInAllowedByXRobotsTagStorage)
	{
		data()->addParsedPage(pageFromResource, StorageType::BlockedByXRobotsTagStorageType, turnaround);
		blockingStoragesSet << StorageType::BlockedByXRobotsTagStorageType;
	}

	if (isNofollowResource && !resourceInDoFollowLinksStorage && !resourceInNoFollowLinksStorage)
	{
		data()->addParsedPage(pageFromResource, StorageType::NofollowLinksStorageType, turnaround);
		blockingStoragesSet << StorageType::NofollowLinksStorageType;
	}

	if (!blockingStoragesSet.isEmpty() && !resourceInBlockedForSEIndexStorage)
	{
		data()->addParsedPage(pageFromResource, StorageType::BlockedForSEIndexingStorageType, turnaround);
		blockingStoragesSet << StorageType::BlockedForSEIndexingStorageType;
	}

	if (isDofollowResource && !resourceInDoFollowLinksStorage)
	{
		if (!resourceInNoFollowLinksStorage)
		{
			// resource is not in the nofollow storage, no need to remove it from other storages
			data()->addParsedPage(pageFromResource, StorageType::DofollowUrlStorageType, turnaround);
		}
		else
		{
			ParsedPagePtr existingPage = data()->parsedPage(pageFromResource, StorageType::NofollowLinksStorageType);

			DEBUG_ASSERT(existingPage);

			data()->addParsedPage(existingPage, StorageType::DofollowUrlStorageType, turnaround);
			data()->removeParsedPage(existingPage, StorageType::NofollowLinksStorageType, turnaround);

			removeFromIndexingBlockingPagesIfNeeded(existingPage, turnaround);
		}
	}

	if (isAllowedByXRobotsTag && !resourceInAllowedByXRobotsTagStorage)
	{
		if (!resourceInBlockedByXRobotsTagStorage)
		{
			// resource is not in the blocked by meta robots storage, no need to remove it from other storages
			data()->addParsedPage(pageFromResource, StorageType::AllowedByXRobotsTagStorageType, turnaround);
		}
		else
		{
			ParsedPagePtr existingPage = data()->parsedPage(pageFromResource, StorageType::BlockedByXRobotsTagStorageType);

			if (!existingPage->isBlockedByMetaRobots)
			{
				DEBUG_ASSERT(existingPage);

				data()->addParsedPage(existingPage, StorageType::AllowedByXRobotsTagStorageType, turnaround);
				data()->removeParsedPage(existingPage, StorageType::BlockedByXRobotsTagStorageType, turnaround);

				removeFromIndexingBlockingPagesIfNeeded(existingPage, turnaround);
			}
		}
	}

	return blockingStoragesSet;
}

void ModelController::removeFromIndexingBlockingPagesIfNeeded(ParsedPagePtr& existingResource, int turnaround)
{
	//
	// we must remove this page from blocked storage type
	// only if this page wasn't blocked by another criteria (x-robots-tag or robots.txt)
	//
	if (!data()->isParsedPageExists(existingResource, BlockedForSEIndexingStorageType))
	{
		return;
	}

	if (!data()->isParsedPageExists(existingResource, BlockedByRobotsTxtStorageType) &&
		!data()->isParsedPageExists(existingResource, BlockedByXRobotsTagStorageType) &&
		!data()->isParsedPageExists(existingResource, NofollowLinksStorageType))
	{
		data()->removeParsedPage(existingResource, StorageType::BlockedForSEIndexingStorageType, turnaround);
	}
}

void ModelController::addToBlockedByXrobotsTagIfNeeded(WorkerResult& workerResult)
{
	if (workerResult.incomingPage()->isBlockedByMetaRobots)
	{
		// even if we following this page by crawler options we should add this page to bloked by meta robots storage
		if (!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::BlockedForSEIndexingStorageType))
		{
			data()->addParsedPage(workerResult.incomingPage(), StorageType::BlockedForSEIndexingStorageType, workerResult.turnaround());
		}

		if (!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::BlockedByXRobotsTagStorageType))
		{
			data()->addParsedPage(workerResult.incomingPage(), StorageType::BlockedByXRobotsTagStorageType, workerResult.turnaround());
		}

		if (data()->isParsedPageExists(workerResult.incomingPage(), StorageType::AllowedByXRobotsTagStorageType))
		{
			data()->removeParsedPage(workerResult.incomingPage(), StorageType::AllowedByXRobotsTagStorageType, workerResult.turnaround());
		}
	}
}

std::pair<ParsedPagePtr, StorageType> ModelController::tryGetPageFromCrawledOrPendingStorage(const ParsedPagePtr& pointer) const
{
	ParsedPagePtr searchPage = data()->parsedPage(pointer, StorageType::CrawledUrlStorageType);

	if (!searchPage)
	{
		return std::make_pair(
			data()->parsedPage(pointer, StorageType::PendingResourcesStorageType),
			StorageType::PendingResourcesStorageType
		);
	}

	return std::make_pair(searchPage, StorageType::CrawledUrlStorageType);
}

ParsedPagePtr ModelController::takeFromCrawledOrPendingStorage(const ParsedPagePtr& pointer) const
{
	const auto[searchPage, storageType] = tryGetPageFromCrawledOrPendingStorage(pointer);

	Q_UNUSED(storageType);

	return searchPage;
}

void ModelController::setLinksForResourcePageAndLoadedPage(ParsedPagePtr& resourcePage, WorkerResult& loadedPage, const ResourceOnPage& resource) const
{
	resourcePage->linksToThisPage.emplace_back(
		ResourceLink
		{
			loadedPage.incomingPage(),
			loadedPage.incomingPage()->url,
			resource.link.linkParameter,
			resource.link.resourceSource,
			resource.link.altOrTitle
		}
	);

	loadedPage.incomingPage()->linksOnThisPage.emplace_back(
		ResourceLink
		{
			resourcePage,
			resourcePage->url,
			resource.link.linkParameter,
			resource.link.resourceSource,
			resource.link.altOrTitle
		}
	);
}

void ModelController::handlePresenceYandexMetricaCounters(WorkerResult& workerResult, bool secondGetRequest)
{
	if (!m_crawlerOptionsData.searchYandexMetricaCounters ||
		secondGetRequest ||
		workerResult.incomingPage()->statusCode != Common::StatusCode::Ok200)
	{
		return;
	}

	std::for_each(workerResult.incomingPage()->missingYandexMetricaCounters.begin(), workerResult.incomingPage()->missingYandexMetricaCounters.end(),
		[&](StorageType targetStorageType) { data()->addParsedPage(workerResult.incomingPage(), targetStorageType, workerResult.turnaround()); });
}

void ModelController::handlePresenceGoogleAnalyticsCounters(WorkerResult& workerResult, bool secondGetRequest)
{
	if (!m_crawlerOptionsData.searchGoogleAnalyticsCounters ||
		secondGetRequest ||
		workerResult.incomingPage()->statusCode != Common::StatusCode::Ok200)
	{
		return;
	}

	std::for_each(workerResult.incomingPage()->missingGoogleAnalyticsCounters.begin(), workerResult.incomingPage()->missingGoogleAnalyticsCounters.end(),
		[&](StorageType targetStorageType) { data()->addParsedPage(workerResult.incomingPage(), targetStorageType, workerResult.turnaround()); });
}

}