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
	for (StorageType type = StorageType::CrawledUrlStorageType; type < StorageType::EndEnumStorageType; type = ++type)
	{
		if (type == StorageType::CrawledUrlStorageType ||
			type == StorageType::PendingResourcesStorageType)
		{
			continue;
		}

		const auto fakeDeleter = [](ParsedPage*) noexcept {};
		ParsedPagePtr pointer(parsedPage, fakeDeleter);

		data()->removeParsedPage(pointer, type);
	}
}

void ModelController::handleWorkerResult(WorkerResult workerResult) noexcept
{
	ASSERT(workerResult.incomingPage()->resourceType >= ResourceType::ResourceHtml &&
		workerResult.incomingPage()->resourceType <= ResourceType::ResourceOther);

#ifdef QT_DEBUG

	if (!workerResult.isRefreshResult())
	{
		const ParsedPagePtr existingPage = data()->parsedPage(workerResult.incomingPage(), StorageType::CrawledUrlStorageType);

		if (existingPage)
		{
			ERRLOG << "Unexpected page:" << workerResult.incomingPage()->url.toDisplayString()
				<< existingPage->url.toDisplayString()
				<< ". This page was already crawled.";

			DEBUG_ASSERT(!existingPage);
		}
	}

#endif

	CrawlerSharedState::instance()->incrementModelControllerCrawledLinksCount();
	
	fixParsedPageResourceType(workerResult.incomingPage());

	if (!resourceShouldBeProcessed(workerResult.incomingPage()->resourceType))
	{
		data()->removeParsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);
		return;
	}

	processParsedPageHtmlResources(workerResult.incomingPage());
	processParsedPageResources(workerResult.incomingPage());
	workerResult.incomingPage()->allResourcesOnPage.clear();

	processParsedPageStatusCode(workerResult.incomingPage());
	processParsedPageUrl(workerResult);

	if (workerResult.incomingPage()->resourceType == ResourceType::ResourceHtml)
	{
		processParsedPageTitle(workerResult.incomingPage());
		processParsedPageMetaDescription(workerResult.incomingPage());
		processParsedPageMetaKeywords(workerResult.incomingPage());
		processParsedPageH1(workerResult.incomingPage());
		processParsedPageH2(workerResult.incomingPage());
	}
	else if (workerResult.incomingPage()->resourceType == ResourceType::ResourceImage)
	{
		processParsedPageImage(workerResult.incomingPage());
	}

	data()->removeParsedPage(workerResult.incomingPage(), StorageType::PendingResourcesStorageType);

	if (!m_linksToPageChanges.changes.empty())
	{
		data()->parsedPageLinksToThisResourceChanged(m_linksToPageChanges);
		m_linksToPageChanges.changes.clear();
	}

	if (workerResult.isRefreshResult())
	{
		emit refreshPageDone();
	}
}

void ModelController::processParsedPageUrl(WorkerResult& workerResult)
{
	const Url url = workerResult.incomingPage()->url;
	const QString urlStr = url.toString();
	
	if (workerResult.isRefreshResult())
	{
		ParsedPagePtr oldPage = data()->parsedPage(workerResult.incomingPage(), StorageType::CrawledUrlStorageType);

		DEBUG_ASSERT(oldPage);

		data()->replaceParsedPage(oldPage, workerResult.incomingPage(), StorageType::CrawledUrlStorageType);
	}
	else
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::CrawledUrlStorageType);
	}

	CrawlerSharedState::instance()->incrementModelControllerAcceptedLinksCount();

	calculatePageLevel(workerResult.incomingPage());

	if (url.host() != m_crawlerOptions.startCrawlingPage.host())
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::ExternalUrlStorageType);
	}

	if (workerResult.incomingPage()->isThisExternalPage)
	{
		return;
	}

	if (urlStr.toLower() != urlStr)
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::UpperCaseUrlStorageType);
	}

	if (urlStr.size() > m_crawlerOptions.limitMaxUrlLength)
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::TooLongUrlStorageType);
	}

	if (workerResult.incomingPage()->linksOnThisPage.size() > m_crawlerOptions.maxLinksCountOnPage)
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::TooManyLinksOnPageStorageType);
	}

	if (workerResult.incomingPage()->hasMetaRefreshTag)
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::ContainsMetaRefreshTagStorageType);
	}

	if (workerResult.incomingPage()->hasFrames)
	{
		data()->addParsedPage(workerResult.incomingPage(), StorageType::ContainsFramesStorageType);
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
		data()->addParsedPage(workerResult.incomingPage(), StorageType::NonAsciiCharacterUrlStorageType);
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
				data()->addParsedPage(workerResult.incomingPage(), StorageType::WwwRedirectionsUrlStorageType);
			}
		}
	}
}

void ModelController::processParsedPageTitle(ParsedPagePtr& incomingPage)
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const Common::StatusCode responseCode = incomingPage->statusCode;
	const QString title = incomingPage->title;
	const QString h1 = incomingPage->firstH1;

	const bool successfulResponseCode = successfulCode(incomingPage);

	if ((title.isEmpty() && successfulResponseCode))
	{
		data()->addParsedPage(incomingPage, StorageType::EmptyTitleUrlStorageType);
	}
	else if (title.size() > m_crawlerOptions.maxTitleLength && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::TooLongTitleUrlStorageType);
	}
	else if (title.size() < m_crawlerOptions.minTitleLength && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::TooShortTitleUrlStorageType);
	}

	if (!title.isEmpty() && successfulResponseCode)
	{
		addDuplicates(incomingPage, StorageType::AllTitlesUrlStorageType, StorageType::DuplicatedTitleUrlStorageType);
		data()->addParsedPage(incomingPage, StorageType::AllTitlesUrlStorageType);
	}

	if (!h1.isEmpty() && h1 == title && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::DuplicatedH1TitleUrlStorageType);
	}

	if (incomingPage->hasSeveralTitleTags && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::SeveralTitleUrlStorageType);
	}
}

void ModelController::processParsedPageMetaDescription(ParsedPagePtr& incomingPage)
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	const int metaDescriptionLength = incomingPage->metaDescription.size();

	if (metaDescriptionLength == 0 && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::EmptyMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength > m_crawlerOptions.maxDescriptionLength && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::TooLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < m_crawlerOptions.minDescriptionLength && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::TooShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0 && successfulResponseCode)
	{
		addDuplicates(incomingPage, StorageType::AllMetaDescriptionsUrlStorageType, StorageType::DuplicatedMetaDescriptionUrlStorageType);
		data()->addParsedPage(incomingPage, StorageType::AllMetaDescriptionsUrlStorageType);
	}

	if (incomingPage->hasSeveralMetaDescriptionTags && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::SeveralMetaDescriptionUrlStorageType);
	}
}

void ModelController::processParsedPageMetaKeywords(ParsedPagePtr& incomingPage)
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	const int metaKeywordsLength = incomingPage->metaKeywords.size();

	if (metaKeywordsLength == 0 && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::EmptyMetaKeywordsUrlStorageType);
	}

	if (metaKeywordsLength > 0 && successfulResponseCode)
	{
		addDuplicates(incomingPage, StorageType::AllMetaKeywordsUrlStorageType, StorageType::DuplicatedMetaKeywordsUrlStorageType);
		data()->addParsedPage(incomingPage, StorageType::AllMetaKeywordsUrlStorageType);
	}


	if (incomingPage->hasSeveralMetaKeywordsTags && successfulResponseCode)
	{
		data()->addParsedPage(incomingPage, StorageType::SeveralMetaKeywordsUrlStorageType);
	}
}

void ModelController::processParsedPageH1(ParsedPagePtr& incomingPage)
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	if (!successfulResponseCode)
	{
		return;
	}

	const int h1Length = incomingPage->firstH1.size();

	if (!h1Length)
	{
		data()->addParsedPage(incomingPage, StorageType::MissingH1UrlStorageType);
	}
	else if (h1Length > m_crawlerOptions.maxH1LengthChars)
	{
		data()->addParsedPage(incomingPage, StorageType::TooLongH1UrlStorageType);
	}

	if (h1Length > 0)
	{
		addDuplicates(incomingPage, StorageType::AllH1UrlStorageType, StorageType::DuplicatedH1UrlStorageType);
		data()->addParsedPage(incomingPage, StorageType::AllH1UrlStorageType);
	}

	if (incomingPage->hasSeveralH1Tags)
	{
		data()->addParsedPage(incomingPage, StorageType::SeveralH1UrlStorageType);
	}
}

void ModelController::processParsedPageH2(ParsedPagePtr& incomingPage)
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	if (!successfulResponseCode)
	{
		return;
	}

	const int h2Length = incomingPage->firstH2.size();

	if (!h2Length)
	{
		data()->addParsedPage(incomingPage, StorageType::MissingH2UrlStorageType);
	}
	else if (h2Length > m_crawlerOptions.maxH2LengthChars)
	{
		data()->addParsedPage(incomingPage, StorageType::TooLongH2UrlStorageType);
	}

	if (h2Length > 0)
	{
		addDuplicates(incomingPage, StorageType::AllH2UrlStorageType, StorageType::DuplicatedH2UrlStorageType);
		data()->addParsedPage(incomingPage, StorageType::AllH2UrlStorageType);
	}

	if (incomingPage->hasSeveralEqualH2Tags)
	{
		data()->addParsedPage(incomingPage, StorageType::SeveralH2UrlStorageType);
	}
}

void ModelController::processParsedPageImage(ParsedPagePtr& incomingPage, bool checkOnlyLastResource)
{
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

	const int sizeKB = incomingPage->pageSizeKilobytes;

	if (sizeKB > m_crawlerOptions.maxImageSizeKb &&
		!data()->isParsedPageExists(incomingPage, StorageType::Over100kbImageStorageType))
	{
		data()->addParsedPage(incomingPage, StorageType::Over100kbImageStorageType);
	}

	std::size_t index = 0;
	const std::size_t lastIndex = incomingPage->linksToThisPage.size() - 1;

	for (const ResourceLink& linkToThisImage : incomingPage->linksToThisPage)
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
				!data()->isParsedPageExists(incomingPage, StorageType::TooLongAltTextImageStorageType))
			{
				data()->addParsedPage(incomingPage, StorageType::TooLongAltTextImageStorageType);
			}

			if (altLength == 0 && !data()->isParsedPageExists(incomingPage, StorageType::MissingAltTextImageStorageType))
			{
				data()->addParsedPage(incomingPage, StorageType::MissingAltTextImageStorageType);
			}
		}
	}
}

void ModelController::processParsedPageStatusCode(ParsedPagePtr& incomingPage)
{
	if (incomingPage->statusCode == Common::StatusCode::NotFound404)
	{
		data()->addParsedPage(incomingPage, StorageType::BrokenLinks);
	}

	if (incomingPage->statusCode >= Common::StatusCode::BadRequest400 &&
		incomingPage->statusCode <= Common::StatusCode::UnsupportedMediaType415)
	{
		data()->addParsedPage(incomingPage, StorageType::Status4xxStorageType);
	}

	if (incomingPage->statusCode >= Common::StatusCode::InternalServerError500 &&
		incomingPage->statusCode <= Common::StatusCode::HttpVersionNotSupported505)
	{
		data()->addParsedPage(incomingPage, StorageType::Status5xxStorageType);
	}

	if (incomingPage->statusCode == Common::StatusCode::MovedPermanently301)
	{
		data()->addParsedPage(incomingPage, StorageType::Status301StorageType);
	}

	if (incomingPage->statusCode == Common::StatusCode::MovedTemporarily302)
	{
		data()->addParsedPage(incomingPage, StorageType::Status302StorageType);
	}
}

void ModelController::processParsedPageHtmlResources(ParsedPagePtr& incomingPage)
{
	if (incomingPage->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	const ParsedPagePtr pendingPageRaw = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);

	incomingPage = mergeTwoPages(pendingPageRaw, incomingPage);

	const StorageType storage = incomingPage->isThisExternalPage ?
		StorageType::ExternalHtmlResourcesStorageType : StorageType::HtmlResourcesStorageType;

	data()->addParsedPage(incomingPage, storage);

	if (incomingPage->isThisExternalPage)
	{
		const bool doFollow = std::any_of(incomingPage->linksToThisPage.begin(), incomingPage->linksToThisPage.end(),
			[](const ResourceLink& link) { return link.linkParameter == LinkParameter::DofollowParameter; });

		if (doFollow)
		{
			DEBUG_ASSERT(!data()->isParsedPageExists(incomingPage, StorageType::ExternalDoFollowUrlResourcesStorageType));
			data()->addParsedPage(incomingPage, StorageType::ExternalDoFollowUrlResourcesStorageType);
		}

		// do not parse resources from an external one
		return;
	}

	if (incomingPage->canonicalUrl.isValid())
	{
		data()->addParsedPage(incomingPage, StorageType::CanonicalUrlResourcesStorageType);
		if (!data()->isParsedPageExists(incomingPage, StorageType::UniqueCanonicalUrlResourcesStorageType))
		{
			data()->addParsedPage(incomingPage, StorageType::UniqueCanonicalUrlResourcesStorageType);
		}
	}

	for (const ResourceOnPage& resource : incomingPage->allResourcesOnPage)
	{
		if (resource.resourceType != ResourceType::ResourceHtml)
		{
			continue;
		}

		ParsedPagePtr resourcePage = std::make_shared<ParsedPage>();
		resourcePage->url = resource.link.url;
		ParsedPagePtr existingResource = data()->parsedPage(resourcePage, StorageType::CrawledUrlStorageType);

		if (!existingResource)
		{
			existingResource = data()->parsedPage(resourcePage, StorageType::PendingResourcesStorageType);
		}

		if (existingResource)
		{
			existingResource->linksToThisPage.emplace_back(ResourceLink { incomingPage, incomingPage->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });

			m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ existingResource, existingResource->linksToThisPage.size() - 1 });
			
			incomingPage->linksOnThisPage.emplace_back(ResourceLink { existingResource, existingResource->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });
		}
		else
		{
			ParsedPagePtr pendingResource = std::make_shared<ParsedPage>();
			pendingResource->url = resource.link.url;

			pendingResource->linksToThisPage.emplace_back(ResourceLink { incomingPage, incomingPage->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });
			
			incomingPage->linksOnThisPage.emplace_back(ResourceLink { pendingResource, pendingResource->url, resource.link.urlParameter,
				resource.link.resourceSource, resource.link.altOrTitle });

			if (!resource.loadAvailability)
			{
				// what if this resource is unavailable not from all pages?
				continue;
			}

			data()->addParsedPage(pendingResource, StorageType::PendingResourcesStorageType);

			DEBUG_ASSERT(data()->isParsedPageExists(pendingResource, StorageType::PendingResourcesStorageType));
		}
	}
}

void ModelController::processParsedPageResources(ParsedPagePtr& incomingPage)
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

	const bool http = PageParserHelpers::isHttpOrHttpsScheme(incomingPage->url);
	const bool externalOrNotHttp = incomingPage->isThisExternalPage || !http;

	if (incomingPage->resourceType != ResourceType::ResourceHtml)
	{
		const ParsedPagePtr pendingPageRaw = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
		incomingPage = mergeTwoPages(pendingPageRaw, incomingPage);

		const ResourceType resourceType = incomingPage->resourceType;

		const StorageType storage = externalOrNotHttp
			? s_externalStorageTypes[resourceType]
			: s_storageTypes[resourceType];
		
		data()->addParsedPage(incomingPage, storage);
	}

	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	for (const ResourceOnPage& resource : incomingPage->allResourcesOnPage)
	{
		const QString resourceDisplayUrl = resource.link.url.toDisplayString();

		if (!resourceShouldBeProcessed(resource.resourceType) || 
			resource.resourceType == ResourceType::ResourceHtml ||
			resourceDisplayUrl.startsWith("javascript:") ||
			resourceDisplayUrl.startsWith("#"))
		{
			continue;
		}

		ParsedPagePtr temporaryResource = std::make_shared<ParsedPage>();
		temporaryResource->url = resource.link.url;

		const bool httpResource = PageParserHelpers::isHttpOrHttpsScheme(resource.link.url);
		const bool externalOrNotHttpResource = PageParserHelpers::isUrlExternal(incomingPage->url, temporaryResource->url) || !httpResource;

		const ResourceType resourceType = resource.resourceType;

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

			if (resource.loadAvailability)
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

		incomingPage->linksOnThisPage.emplace_back(ResourceLink { newOrExistingResource, newOrExistingResource->url, resource.link.urlParameter,
			resource.link.resourceSource, resource.link.altOrTitle });
		
		newOrExistingResource->linksToThisPage.emplace_back(ResourceLink { incomingPage, incomingPage->url, resource.link.urlParameter,
			resource.link.resourceSource, resource.link.altOrTitle });
		
		m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ newOrExistingResource, newOrExistingResource->linksToThisPage.size() - 1 });

		newOrExistingResource->resourceType = resource.resourceType;

		// special case: parse image resource again because it can have now empty or too short/long alt text
		if (existingImageResource)
		{
			processParsedPageImage(newOrExistingResource, true);
		}
	}
}

void ModelController::fixParsedPageResourceType(ParsedPagePtr& incomingPage) const noexcept
{
	ParsedPagePtr pendingResource = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
	if (pendingResource && pendingResource->resourceType != ResourceType::ResourceHtml)
	{
		incomingPage->resourceType = pendingResource->resourceType;
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
		if (parent->statusCode != Common::StatusCode::Ok200)
		{
			continue;
		}
		
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

void ModelController::addDuplicates(const ParsedPagePtr& incomingPage, StorageType lookupStorage, StorageType destStorage)
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
		const std::vector<ParsedPagePtr> allDuplicates = 
			data()->allParsedPages(incomingPage, lookupStorage, alwaysTruePredicate);

		for (const ParsedPagePtr& duplicate : allDuplicates)
		{
			DEBUG_ASSERT(!duplicate->storages[static_cast<std::size_t>(destStorage)]);
			
			data()->addParsedPage(duplicate, destStorage);
		}

		data()->addParsedPage(incomingPage, destStorage);
	}
}

}