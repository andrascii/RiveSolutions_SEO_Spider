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

void ModelController::addParsedPage(ParsedPagePtr incomingPage) noexcept
{
	ASSERT(incomingPage->resourceType >= ResourceType::ResourceHtml &&
		incomingPage->resourceType <= ResourceType::ResourceOther);

	CrawlerSharedState::instance()->incrementModelControllerCrawledLinksCount();

	fixParsedPageResourceType(incomingPage);

	if (!resourceShouldBeProcessed(incomingPage->resourceType))
	{
		data()->removeParsedPage(incomingPage, StorageType::PendingResourcesStorageType);
		return;
	}

	processParsedPageHtmlResources(incomingPage);
	processParsedPageResources(incomingPage);
	incomingPage->allResourcesOnPage.clear();

	processParsedPageStatusCode(incomingPage);
	processParsedPageUrl(incomingPage);

	if (incomingPage->resourceType == ResourceType::ResourceHtml)
	{
		// page
		processParsedPageTitle(incomingPage);
		processParsedPageMetaDescription(incomingPage);
		processParsedPageMetaKeywords(incomingPage);
		processParsedPageH1(incomingPage);
		processParsedPageH2(incomingPage);
	}
	else if (incomingPage->resourceType == ResourceType::ResourceImage)
	{
		// image resource
		processParsedPageImage(incomingPage);
	}

	data()->removeParsedPage(incomingPage, StorageType::PendingResourcesStorageType);

	if (!m_linksToPageChanges.changes.empty())
	{
		data()->parsedPageLinksToThisResourceChanged(m_linksToPageChanges);
		m_linksToPageChanges.changes.clear();
	}

	//data()->addParsedPage(incomingPage, StorageType::CrawledUrlStorageType);
	//CrawlerSharedState::instance()->incrementModelControllerAcceptedLinksCount();
}

void ModelController::addParsedPages(std::vector<ParsedPagePtr> incomingPages) noexcept
{
	for (ParsedPagePtr page : incomingPages)
	{
		addParsedPage(page);
	}
}

void ModelController::processParsedPageUrl(ParsedPagePtr& incomingPage)
{
	const Url url = incomingPage->url;
	const QString urlStr = url.toString();
	data()->addParsedPage(incomingPage, StorageType::CrawledUrlStorageType);
	CrawlerSharedState::instance()->incrementModelControllerAcceptedLinksCount();
	calculatePageLevel(incomingPage);

	if (url.host() != m_crawlerOptions.host.host())
	{
		data()->addParsedPage(incomingPage, StorageType::ExternalUrlStorageType);
	}

	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	if (urlStr.toLower() != urlStr)
	{
		data()->addParsedPage(incomingPage, StorageType::UpperCaseUrlStorageType);
	}

	if (urlStr.size() > m_crawlerOptions.limitMaxUrlLength)
	{
		data()->addParsedPage(incomingPage, StorageType::TooLongUrlStorageType);
	}

	if (incomingPage->linksOnThisPage.size() > m_crawlerOptions.maxLinksCountOnPage)
	{
		data()->addParsedPage(incomingPage, StorageType::TooManyLinksOnPageStorageType);
	}

	if (incomingPage->hasMetaRefreshTag)
	{
		data()->addParsedPage(incomingPage, StorageType::ContainsMetaRefreshTagStorageType);
	}

	if (incomingPage->hasFrames)
	{
		data()->addParsedPage(incomingPage, StorageType::ContainsFramesStorageType);
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
		data()->addParsedPage(incomingPage, StorageType::NonAsciiCharacterUrlStorageType);
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

	if (h2Length > 0 && incomingPage->hasSeveralEqualH2Tags)
	{
		data()->addParsedPage(incomingPage, StorageType::DuplicatedH2UrlStorageType);
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
		// if it is not an html resource, just exit
		return;
	}

	// 1. get from pending resources if exists
	const ParsedPagePtr pendingPageRaw = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);

	// 2. if it's really html resource and pending page exists - merge two pages
	incomingPage = mergeTwoPages(pendingPageRaw, incomingPage);

	// 3. add this page to html resources storage
	const StorageType storage = incomingPage->isThisExternalPage ?
		StorageType::ExternalHtmlResourcesStorageType : StorageType::HtmlResourcesStorageType;

	data()->addParsedPage(incomingPage, storage);

	if (incomingPage->isThisExternalPage)
	{
		// do not parse resources from an external one
		return;
	}

	for (const RawResourceOnPage& resource : incomingPage->allResourcesOnPage)
	{
		if (resource.resourceType != ResourceType::ResourceHtml)
		{
			continue;
		}

		ParsedPagePtr resourcePage = std::make_shared<ParsedPage>();
		resourcePage->url = resource.thisResourceLink.url;
		ParsedPagePtr existingResource = data()->parsedPage(resourcePage, StorageType::CrawledUrlStorageType);

		if (!existingResource)
		{
			existingResource = data()->parsedPage(resourcePage, StorageType::PendingResourcesStorageType);
		}

		if (existingResource)
		{
			existingResource->linksToThisPage.emplace_back(ResourceLink { incomingPage, incomingPage->url, resource.thisResourceLink.urlParameter,
				resource.thisResourceLink.resourceSource, resource.thisResourceLink.altOrTitle });
			
			m_linksToPageChanges.changes.emplace_back(LinksToThisResourceChanges::Change{ existingResource, existingResource->linksToThisPage.size() - 1 });
			
			incomingPage->linksOnThisPage.emplace_back(ResourceLink { existingResource, existingResource->url, resource.thisResourceLink.urlParameter,
				resource.thisResourceLink.resourceSource, resource.thisResourceLink.altOrTitle });
		}
		else
		{
			ParsedPagePtr pendingResource = std::make_shared<ParsedPage>();
			pendingResource->url = resource.thisResourceLink.url;

			pendingResource->linksToThisPage.emplace_back(ResourceLink { incomingPage, incomingPage->url, resource.thisResourceLink.urlParameter,
				resource.thisResourceLink.resourceSource, resource.thisResourceLink.altOrTitle });
			
			incomingPage->linksOnThisPage.emplace_back(ResourceLink { pendingResource, pendingResource->url, resource.thisResourceLink.urlParameter,
				resource.thisResourceLink.resourceSource, resource.thisResourceLink.altOrTitle });

			if (!resource.loadAvailability)
			{
				continue;
			}

			data()->addParsedPage(pendingResource, StorageType::PendingResourcesStorageType);

			DEBUG_ASSERT(data()->isParsedPageExists(pendingResource, StorageType::PendingResourcesStorageType));
		}
	}
}

void ModelController::processParsedPageResources(ParsedPagePtr& incomingPage)
{
	constexpr int storageTypes[][1]
	{
		static_cast<int>(ResourceType::ResourceImage), { static_cast<int>(StorageType::ImageResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceJavaScript), { static_cast<int>(StorageType::JavaScriptResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceStyleSheet), { static_cast<int>(StorageType::StyleSheetResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceFlash), { static_cast<int>(StorageType::FlashResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceVideo), { static_cast<int>(StorageType::VideoResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceOther), { static_cast<int>(StorageType::OtherResourcesStorageType) },
	};

	constexpr int externalStorageTypes[][1]
	{
		static_cast<int>(ResourceType::ResourceImage), { static_cast<int>(StorageType::ExternalImageResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceJavaScript), { static_cast<int>(StorageType::ExternalJavaScriptResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceStyleSheet), { static_cast<int>(StorageType::ExternalStyleSheetResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceFlash), { static_cast<int>(StorageType::ExternalFlashResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceVideo), { static_cast<int>(StorageType::ExternalVideoResourcesStorageType) },
		static_cast<int>(ResourceType::ResourceOther), { static_cast<int>(StorageType::ExternalOtherResourcesStorageType) },
	};

	const bool http = PageParserHelpers::isHttpOrHttpsScheme(incomingPage->url);
	const bool externalOrNotHttp = incomingPage->isThisExternalPage || !http;

	if (incomingPage->resourceType != ResourceType::ResourceHtml)
	{
		const ParsedPagePtr pendingPageRaw = data()->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
		incomingPage = mergeTwoPages(pendingPageRaw, incomingPage);
		
		const int resourceType = static_cast<int>(incomingPage->resourceType);

		const StorageType storage = static_cast<StorageType>(externalOrNotHttp ?
			externalStorageTypes[resourceType][0] : 
			storageTypes[resourceType][0]
		);
		
		data()->addParsedPage(incomingPage, storage);
	}

	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	for (const RawResourceOnPage& resource : incomingPage->allResourcesOnPage)
	{
		const QString resourceDisplayUrl = resource.thisResourceLink.url.toDisplayString();

		if (!resourceShouldBeProcessed(resource.resourceType) || 
			resource.resourceType == ResourceType::ResourceHtml ||
			resourceDisplayUrl.startsWith("javascript:") ||
			resourceDisplayUrl.startsWith("#"))
		{
			continue;
		}

		ParsedPagePtr temporaryResource = std::make_shared<ParsedPage>();
		temporaryResource->url = resource.thisResourceLink.url;

		const bool httpResource = PageParserHelpers::isHttpOrHttpsScheme(resource.thisResourceLink.url);
		const bool externalOrNotHttpResource = PageParserHelpers::isUrlExternal(incomingPage->url, temporaryResource->url) || !httpResource;

		const int resourceType = static_cast<int>(resource.resourceType);

		const StorageType storage = static_cast<StorageType>(externalOrNotHttpResource ?
			externalStorageTypes[resourceType][0] : 
			storageTypes[resourceType][0]
		);

		ParsedPagePtr newOrExistingResource = data()->parsedPage(temporaryResource, storage);
		
		const bool existingImageResource = newOrExistingResource && 
			newOrExistingResource->resourceType == ResourceType::ResourceImage &&
			resource.thisResourceLink.resourceSource == ResourceSource::SourceTagImg;

		if (!newOrExistingResource)
		{
			newOrExistingResource = data()->parsedPage(temporaryResource, StorageType::PendingResourcesStorageType);
		}

		if (!newOrExistingResource)
		{
			newOrExistingResource = temporaryResource;

			if (resource.loadAvailability)
			{
				data()->addParsedPage(newOrExistingResource,
					httpResource ? StorageType::PendingResourcesStorageType : storage);
			}
		}

		incomingPage->linksOnThisPage.emplace_back(ResourceLink { newOrExistingResource, newOrExistingResource->url, resource.thisResourceLink.urlParameter,
			resource.thisResourceLink.resourceSource, resource.thisResourceLink.altOrTitle });
		
		newOrExistingResource->linksToThisPage.emplace_back(ResourceLink { incomingPage, incomingPage->url, resource.thisResourceLink.urlParameter,
			resource.thisResourceLink.resourceSource, resource.thisResourceLink.altOrTitle });
		
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
			level = parent->pageLevel == 1 && parent->url.compare(m_crawlerOptions.host)
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
			candidatePage->url.canonizedUrlStr() != // TODO: remove these two lines
			page->url.canonizedUrlStr() &&
		
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