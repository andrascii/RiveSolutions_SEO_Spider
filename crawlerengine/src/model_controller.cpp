#include "model_controller.h"
#include "unordered_data_collection.h"
#include "page_parser_helpers.h"
#include "crawler_shared_state.h"

namespace
{

using CrawlerEngine::ParsedPagePtr;

ParsedPagePtr mergeTwoPages(ParsedPagePtr existingPage, ParsedPagePtr incomingParsedPage)
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

	*existingPage = *incomingParsedPage;

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

void ModelController::setWebCrawlerOptions(const CrawlerOptions& options)
{
	m_crawlerOptions = options;
}

void ModelController::clearData()
{
	data()->clearData();

	CrawlerSharedState::instance()->setModelControllerAcceptedLinksCount(0);
	CrawlerSharedState::instance()->setModelControllerCrawledLinksCount(0);
}

void ModelController::preparePageForRefresh(ParsedPage* parsedPage)
{
	const auto fakeDeleter = [](ParsedPage*) noexcept {};
	ParsedPagePtr pointer(parsedPage, fakeDeleter);

	data()->prepareCollectionForRefreshPage(pointer);
}

void ModelController::handleWorkerResult(WorkerResult workerResult) noexcept
{
	ASSERT(workerResult.incomingPage()->resourceType >= ResourceType::ResourceHtml &&
		workerResult.incomingPage()->resourceType <= ResourceType::ResourceOther);

	bool secondGetRequest = false;
	if (!workerResult.isRefreshResult())
	{
		const ParsedPagePtr existingPage = data()->parsedPage(workerResult.incomingPage(), StorageType::CrawledUrlStorageType);

		if (existingPage)
		{
			if (workerResult.requestType() == DownloadRequestType::RequestTypeHead)
			{
				CrawlerSharedState::instance()->incrementModelControllerCrawledLinksCount();
				return;
			}
			
			secondGetRequest = true;
		}
		
		workerResult.incomingPage() = mergeTwoPages(existingPage, workerResult.incomingPage());
	}

	CrawlerSharedState::instance()->incrementModelControllerCrawledLinksCount();
	
	fixParsedPageResourceType(workerResult.incomingPage());

	if (!resourceShouldBeProcessed(workerResult.incomingPage()->resourceType))
	{
		data()->removeParsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);
		return;
	}

	processParsedPageHtmlResources(workerResult, secondGetRequest);
	processParsedPageResources(workerResult, secondGetRequest);
	workerResult.incomingPage()->allResourcesOnPage.clear();

	processParsedPageStatusCode(workerResult, secondGetRequest);
	processParsedPageUrl(workerResult, secondGetRequest);

	if (workerResult.incomingPage()->resourceType == ResourceType::ResourceHtml)
	{
		processParsedPageTitle(workerResult, secondGetRequest);
		processParsedPageMetaDescription(workerResult, secondGetRequest);
		processParsedPageMetaKeywords(workerResult, secondGetRequest);
		processParsedPageH1(workerResult, secondGetRequest);
		processParsedPageH2(workerResult, secondGetRequest);
	}
	else if (workerResult.incomingPage()->resourceType == ResourceType::ResourceImage)
	{
		processParsedPageImage(workerResult, false, secondGetRequest);
	}

	data()->removeParsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);

	if (!m_linksToPageChanges.changes.empty())
	{
		data()->parsedPageLinksToThisResourceChanged(m_linksToPageChanges);
		m_linksToPageChanges.changes.clear();
	}

	INFOLOG << workerResult.incomingPage()->url.canonizedUrlStr();

	if (!workerResult.isRefreshResult())
	{
		DEBUG_ASSERT(!workerResult.incomingPage()->linksToThisPage.empty() ||
			data()->size(StorageType::CrawledUrlStorageType) == 1);

		DEBUG_ASSERT(!workerResult.incomingPage()->redirectedUrl.isValid() ||
			!workerResult.incomingPage()->isThisExternalPage ||
			workerResult.incomingPage()->linksOnThisPage.size() == 1 &&
			!workerResult.incomingPage()->linksOnThisPage.front().resource.expired());
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
		ParsedPagePtr oldPage = data()->parsedPage(workerResult.incomingPage(), StorageType::CrawledUrlStorageType);

		ASSERT(oldPage);

		data()->replaceParsedPage(oldPage, workerResult.incomingPage(), StorageType::CrawledUrlStorageType);
	}
	else
	{
		data()->addParsedPage(workerResult, StorageType::CrawledUrlStorageType);
	}

	CrawlerSharedState::instance()->incrementModelControllerAcceptedLinksCount();

	calculatePageLevel(workerResult.incomingPage());

	if (url.host() != m_crawlerOptions.startCrawlingPage.host())
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

	if (urlStr.size() > m_crawlerOptions.limitMaxUrlLength)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongUrlStorageType);
	}

	if (workerResult.incomingPage()->linksOnThisPage.size() > m_crawlerOptions.maxLinksCountOnPage)
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
	else if (title.size() > m_crawlerOptions.maxTitleLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongTitleUrlStorageType);
	}
	else if (title.size() < m_crawlerOptions.minTitleLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooShortTitleUrlStorageType);
	}

	if (!title.isEmpty() && successfulResponseCode)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllTitlesUrlStorageType, StorageType::DuplicatedTitleUrlStorageType);
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
	else if (metaDescriptionLength > m_crawlerOptions.maxDescriptionLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < m_crawlerOptions.minDescriptionLength && successfulResponseCode)
	{
		data()->addParsedPage(workerResult, StorageType::TooShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0 && successfulResponseCode)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllMetaDescriptionsUrlStorageType, StorageType::DuplicatedMetaDescriptionUrlStorageType);
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
		addDuplicates(workerResult.incomingPage(), StorageType::AllMetaKeywordsUrlStorageType, StorageType::DuplicatedMetaKeywordsUrlStorageType);
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
	else if (h1Length > m_crawlerOptions.maxH1LengthChars)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongH1UrlStorageType);
	}

	if (h1Length > 0)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllH1UrlStorageType, StorageType::DuplicatedH1UrlStorageType);
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
	else if (h2Length > m_crawlerOptions.maxH2LengthChars)
	{
		data()->addParsedPage(workerResult, StorageType::TooLongH2UrlStorageType);
	}

	if (h2Length > 0)
	{
		addDuplicates(workerResult.incomingPage(), StorageType::AllH2UrlStorageType, StorageType::DuplicatedH2UrlStorageType);
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

	if (sizeKB > m_crawlerOptions.maxImageSizeKb &&
		!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::Over100kbImageStorageType))
	{
		data()->addParsedPage(workerResult, StorageType::Over100kbImageStorageType);
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

			if (altLength > m_crawlerOptions.maxImageAltTextChars && 
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

	if (workerResult.incomingPage()->statusCode == Common::StatusCode::NotFound404)
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
}

void ModelController::processParsedPageHtmlResources(WorkerResult& workerResult, bool secondGetRequest)
{
	if (workerResult.incomingPage()->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	const ParsedPagePtr pendingPage = data()->parsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);

	workerResult.incomingPage() = mergeTwoPages(pendingPage, workerResult.incomingPage());

	const StorageType storage = workerResult.incomingPage()->isThisExternalPage ?
		StorageType::ExternalHtmlResourcesStorageType : StorageType::HtmlResourcesStorageType;

	if (!secondGetRequest)
	{
		data()->addParsedPage(workerResult, storage);
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
		data()->addParsedPage(workerResult, StorageType::CanonicalUrlResourcesStorageType);

		if (!data()->isParsedPageExists(workerResult.incomingPage(), StorageType::UniqueCanonicalUrlResourcesStorageType))
		{
			data()->addParsedPage(workerResult, StorageType::UniqueCanonicalUrlResourcesStorageType);
		}
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
		ParsedPagePtr existingResource = data()->parsedPage(resourcePage, StorageType::CrawledUrlStorageType);

		if (!existingResource)
		{
			existingResource = data()->parsedPage(resourcePage, StorageType::PendingResourcesStorageType);
		}

		if (existingResource)
		{
			existingResource->linksToThisPage.emplace_back(ResourceLink { workerResult.incomingPage(), workerResult.incomingPage()->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });

			m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ existingResource, existingResource->linksToThisPage.size() - 1 });
			
			workerResult.incomingPage()->linksOnThisPage.emplace_back(ResourceLink { existingResource, existingResource->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });
		}
		else
		{
			ParsedPagePtr pendingResource = parsedPageFromResource(resource);

			pendingResource->linksToThisPage.emplace_back(ResourceLink { workerResult.incomingPage(), workerResult.incomingPage()->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });
			
			workerResult.incomingPage()->linksOnThisPage.emplace_back(ResourceLink { pendingResource, pendingResource->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });

			const bool isBlockedByXRobotsTag = resource.permission == Permission::PermissionBlockedByMetaRobotsRules;
			const bool isBlockedByRobotsTxt = resource.permission == Permission::PermissionBlockedByRobotsTxtRules;
			const bool isNofollowResource = resource.link.urlParameter == LinkParameter::NofollowParameter;
			const bool isThisNofollowResourceExists = data()->isParsedPageExists(pendingResource, StorageType::NofollowLinksStorageType);
			const bool isThisDofollowResourceExists = data()->isParsedPageExists(pendingResource, StorageType::DofollowUrlStorageType);

			bool isResourceBlockedForIndexing = false;

			if (isBlockedByRobotsTxt || isBlockedByXRobotsTag || isNofollowResource)
			{
				data()->addParsedPage(pendingResource, StorageType::BlockedForSEIndexingStorageType);
				isResourceBlockedForIndexing = true;

				if (isBlockedByXRobotsTag)
				{
					data()->addParsedPage(pendingResource, StorageType::BlockedByXRobotsTagStorageType);
					isResourceBlockedForIndexing = true;
				}
				else if (isBlockedByRobotsTxt)
				{
					data()->addParsedPage(pendingResource, StorageType::BlockedByRobotsTxtStorageType);
					isResourceBlockedForIndexing = true;
				}
			}

			if (isNofollowResource)
			{
				if (!isThisDofollowResourceExists && !isThisNofollowResourceExists)
				{
					data()->addParsedPage(pendingResource, StorageType::NofollowLinksStorageType);
					isResourceBlockedForIndexing = true;
				}
			}
			else
			{
				if (!isThisDofollowResourceExists)
				{
					data()->addParsedPage(pendingResource, StorageType::DofollowUrlStorageType);
				}
			}

			if (!isResourceBlockedForIndexing &&
				resource.permission != Permission::PermissionAllowed &&
				resource.permission != Permission::PermissionBlockedByRobotsTxtRules &&
				resource.permission != Permission::PermissionNotHttpLinkNotAllowed)
			{
				continue;
			}

			data()->addParsedPage(pendingResource, StorageType::PendingResourcesStorageType);

			DEBUG_ASSERT(data()->isParsedPageExists(pendingResource, StorageType::PendingResourcesStorageType));
		}
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
		workerResult.incomingPage() = mergeTwoPages(pendingPageRaw, workerResult.incomingPage());

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

		const ResourceType resourceType = workerResult.incomingPage()->resourceType != ResourceType::ResourceHtml ? 
			workerResult.incomingPage()->resourceType :
			resource.resourceType;

		if ((!resourceShouldBeProcessed(resource.resourceType) && 
			resource.link.resourceSource != ResourceSource::SourceRedirectUrl) ||
			resourceType == ResourceType::ResourceHtml ||
			resourceDisplayUrl.startsWith("javascript:") ||
			resourceDisplayUrl.startsWith("#"))
		{
			continue;
		}

		if (workerResult.incomingPage()->isThisExternalPage &&
			resource.link.resourceSource != ResourceSource::SourceRedirectUrl)
		{
			continue;
		}

		ParsedPagePtr temporaryResource = parsedPageFromResource(resource);

		const bool httpResource = PageParserHelpers::isHttpOrHttpsScheme(resource.link.url);
		const bool externalOrNotHttpResource = PageParserHelpers::isUrlExternal(workerResult.incomingPage()->url, temporaryResource->url) || !httpResource;

		const StorageType storage = externalOrNotHttpResource ?
			s_externalStorageTypes[resourceType] : 
			s_storageTypes[resourceType];

		ParsedPagePtr newOrExistingResource = data()->parsedPage(temporaryResource, storage);
		
		const bool existingImageResource = newOrExistingResource && 
			newOrExistingResource->resourceType == ResourceType::ResourceImage &&
			resource.link.resourceSource == ResourceSource::SourceTagImg;

		if (!newOrExistingResource)
		{
			newOrExistingResource = data()->parsedPage(temporaryResource, StorageType::PendingResourcesStorageType);
		}

		if (!newOrExistingResource)
		{
			newOrExistingResource = temporaryResource;

			if (httpResource &&
				(resource.permission == Permission::PermissionAllowed ||
				resource.permission == Permission::PermissionBlockedByRobotsTxtRules))
			{
				// what if this resource is unavailable not from all pages?
				data()->addParsedPage(newOrExistingResource,
					httpResource ? StorageType::PendingResourcesStorageType : storage);
			}
			else if (!httpResource)
			{
				data()->addParsedPage(newOrExistingResource, storage);
			}
		}

		workerResult.incomingPage()->linksOnThisPage.emplace_back(ResourceLink { newOrExistingResource, newOrExistingResource->url, resource.link.urlParameter,
			resource.link.resourceSource, resource.link.altOrTitle });
		
		newOrExistingResource->linksToThisPage.emplace_back(ResourceLink { workerResult.incomingPage(), workerResult.incomingPage()->url, resource.link.urlParameter,
			resource.link.resourceSource, resource.link.altOrTitle });
		
		m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ newOrExistingResource, newOrExistingResource->linksToThisPage.size() - 1 });

		newOrExistingResource->resourceType = resourceType;

		// special case: parse image resource again because it can have now empty or too short/long alt text
		if (existingImageResource)
		{
			WorkerResult result(newOrExistingResource, workerResult.isRefreshResult(), workerResult.requestType());
			processParsedPageImage(result, true, secondGetRequest);
		}
	}
}

void ModelController::fixParsedPageResourceType(ParsedPagePtr& incomingPage) const noexcept
{
	ParsedPagePtr pendingResource = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
	if (pendingResource && pendingResource->resourceType != ResourceType::ResourceHtml)
	{
		incomingPage->resourceType = pendingResource->resourceType;
		if (pendingResource->linksToThisPage.size() == 1 && 
			pendingResource->linksToThisPage.begin()->resourceSource == ResourceSource::SourceRedirectUrl &&
			pendingResource->linksToThisPage.begin()->resource.expired())
		{
			incomingPage->resourceType = pendingResource->linksToThisPage.begin()->resource.lock()->resourceType;
		}
	}
}

bool ModelController::resourceShouldBeProcessed(ResourceType resourceType) const noexcept
{
	switch (resourceType)
	{
		case ResourceType::ResourceHtml:
		{
			return true;
		}
		case ResourceType::ResourceImage:
		{
			return m_crawlerOptions.parserTypeFlags.testFlag(ImagesResourcesParserType);
		}
		case ResourceType::ResourceJavaScript:
		{
			return m_crawlerOptions.parserTypeFlags.testFlag(JavaScriptResourcesParserType);
		}
		case ResourceType::ResourceStyleSheet:
		{
			return m_crawlerOptions.parserTypeFlags.testFlag(CssResourcesParserType);
		}
		case ResourceType::ResourceFlash:
		{
			return m_crawlerOptions.parserTypeFlags.testFlag(FlashResourcesParserType);
		}
		case ResourceType::ResourceVideo:
		{
			return m_crawlerOptions.parserTypeFlags.testFlag(VideoResourcesParserType);
		}
		case ResourceType::ResourceOther:
		{
			return m_crawlerOptions.parserTypeFlags.testFlag(OtherResourcesParserType);
		}
		default:
		{
			ASSERT(!"Unexpected resource type");
			break;
		}
	}

	return false;
}

void ModelController::calculatePageLevel(ParsedPagePtr& incomingPage) const noexcept
{
	int level = 1;
	bool hasParentResources = false;
	for (const ResourceLink& link : incomingPage->linksToThisPage)
	{
		if (link.resource.expired())
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
			level = parent->pageLevel == 1 && parent->url.compare(m_crawlerOptions.startCrawlingPage)
				? 1 : parent->pageLevel + 1;
		}
	}


	const bool updateChildren = incomingPage->pageLevel > level;

	ASSERT(level != invalidPageLevel);

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

void ModelController::addDuplicates(ParsedPagePtr& incomingPage, StorageType lookupStorage, StorageType destStorage)
{
	if (data()->isParsedPageExists(incomingPage, destStorage))
	{
		data()->addParsedPage(incomingPage, destStorage);
		return;
	}

	const auto predicate = [&page = incomingPage](const ParsedPagePtr& candidatePage)
	{
		return 
			// discard pages that are different only by trailing slash
			candidatePage->url.canonizedUrlStr() != 
			page->url.canonizedUrlStr() &&
		
			// and discard pages with the same canonical url
			(candidatePage->canonicalUrl.canonizedUrlStr().isEmpty() ||
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
			
			data()->addParsedPage(duplicate, destStorage);
		}

		data()->addParsedPage(incomingPage, destStorage);
	}
}

ParsedPagePtr ModelController::parsedPageFromResource(const ResourceOnPage& resource) const
{
	ParsedPagePtr parsedPage = std::make_shared<ParsedPage>();
	parsedPage->url = resource.link.url;

	return parsedPage;
}

}