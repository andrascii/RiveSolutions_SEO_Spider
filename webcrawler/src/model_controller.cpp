#include "model_controller.h"
#include "unordered_data_collection.h"
#include "page_parser_helpers.h"

namespace
{

using WebCrawler::ParsedPagePtr;

ParsedPagePtr mergeTwoPages(ParsedPagePtr existingPage, ParsedPagePtr newPage)
{
	if (!existingPage)
	{
		return newPage;
	}

	newPage->linksToThisPage.insert(
		std::begin(newPage->linksToThisPage),
		std::begin(existingPage->linksToThisPage),
		std::end(existingPage->linksToThisPage)
	);

	*existingPage = *newPage;

	// we should use the original PageRawPtr 
	// because there are pages containing links to it
	return existingPage;
}

}

namespace WebCrawler
{

ModelController::ModelController(QObject* parent, QThread* sequencedDataCollectionThread)
	: QObject(parent)
	, m_data(new UnorderedDataCollection(this, sequencedDataCollectionThread))
{
}

ModelController::~ModelController()
{
	
}

void ModelController::setWebCrawlerOptions(const CrawlerOptions& options)
{
	m_crawlerOptions = options;
}

void ModelController::addParsedPage(ParsedPagePtr parsedPagePtr) noexcept
{
	ASSERT(parsedPagePtr->resourceType >= ResourceType::ResourceHtml &&
		parsedPagePtr->resourceType <= ResourceType::ResourceOther)

	fixParsedPageResourceType(parsedPagePtr);
	processParsedPageHtmlResources(parsedPagePtr);
	processParsedPageResources(parsedPagePtr);
	parsedPagePtr->allResourcesOnPage.clear();

	processParsedPageStatusCode(parsedPagePtr);

	if (parsedPagePtr->resourceType == ResourceType::ResourceHtml)
	{
		// page
		processParsedPageUrl(parsedPagePtr);
		processParsedPageTitle(parsedPagePtr);
		processParsedPageMetaDescription(parsedPagePtr);
		processParsedPageMetaKeywords(parsedPagePtr);
		processParsedPageH1(parsedPagePtr);
		processParsedPageH2(parsedPagePtr);
	}
	else
	{
		// image resource
		processParsedPageImage(parsedPagePtr);
	}

	m_data->removeParsedPage(parsedPagePtr, StorageType::PendingResourcesStorageType);
}

const UnorderedDataCollection* ModelController::data() const noexcept
{
	return m_data;
}

UnorderedDataCollection* ModelController::data() noexcept
{
	return m_data;
}

void ModelController::processParsedPageUrl(ParsedPagePtr parsedPagePtr) noexcept
{
	const QUrl url = parsedPagePtr->url;
	const QString urlStr = url.toString();

	m_data->addParsedPage(parsedPagePtr, StorageType::CrawledUrlStorageType);

	if (url.host() != m_crawlerOptions.host.host())
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::ExternalUrlStorageType);
	}

	if (parsedPagePtr->isThisExternalPage)
	{
		return;
	}

	if (urlStr.toLower() != urlStr)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::UpperCaseUrlStorageType);
	}

	if (urlStr.size() > m_crawlerOptions.limitMaxUrlLength)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongUrlStorageType);
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
		m_data->addParsedPage(parsedPagePtr, StorageType::NonAsciiCharacterUrlStorageType);
	}
}

void ModelController::processParsedPageTitle(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->isThisExternalPage)
	{
		return;
	}

	int statusCode = parsedPagePtr->statusCode;
	const QString title = parsedPagePtr->title;
	const QString h1 = parsedPagePtr->firstH1;

	//
	// Replace 301 and 302 constants
	//
	if (title.isEmpty() ||
		statusCode == 301 || statusCode == 302)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::EmptyTitleUrlStorageType);
	}
	else if (title.size() > m_crawlerOptions.maxTitleLength)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongTitleUrlStorageType);
	}
	else if (title.size() < m_crawlerOptions.minTitleLength)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongTitleUrlStorageType);
	}

	if (!title.isEmpty())
	{
		if (m_data->isParsedPageExists(parsedPagePtr, StorageType::AllTitlesUrlStorageType))
		{
			ParsedPagePtr duplicate = m_data->parsedPage(parsedPagePtr, StorageType::AllTitlesUrlStorageType);
			m_data->addParsedPage(parsedPagePtr, StorageType::DuplicatedTitleUrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedTitleUrlStorageType);
		}
		else
		{
			m_data->addParsedPage(parsedPagePtr, StorageType::AllTitlesUrlStorageType);
		}
	}

	if (!h1.isEmpty() && h1 == title)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::DuplicatedH1TitleUrlStorageType);
	}

	if (parsedPagePtr->hasSeveralTitleTags)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::SeveralTitleUrlStorageType);
	}
}

void ModelController::processParsedPageMetaDescription(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->isThisExternalPage)
	{
		return;
	}

	const int metaDescriptionLength = parsedPagePtr->metaDescription.size();

	if (metaDescriptionLength == 0)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::EmptyMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength > m_crawlerOptions.maxDescriptionLength)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < m_crawlerOptions.minDescriptionLength)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0)
	{
		if (m_data->isParsedPageExists(parsedPagePtr, StorageType::AllMetaDescriptionsUrlStorageType))
		{
			ParsedPagePtr duplicate = m_data->parsedPage(parsedPagePtr, StorageType::AllMetaDescriptionsUrlStorageType);
			m_data->addParsedPage(parsedPagePtr, StorageType::DuplicatedMetaDescriptionUrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedMetaDescriptionUrlStorageType);
		}
		else
		{
			m_data->addParsedPage(parsedPagePtr, StorageType::AllMetaDescriptionsUrlStorageType);
		}
	}

	if (parsedPagePtr->hasSeveralMetaDescriptionTags)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::SeveralMetaDescriptionUrlStorageType);
	}
}

void ModelController::processParsedPageMetaKeywords(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->isThisExternalPage)
	{
		return;
	}

	const int metaKeywordsLength = parsedPagePtr->metaKeywords.size();

	if (metaKeywordsLength == 0)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::EmptyMetaKeywordsUrlStorageType);
	}

	if (metaKeywordsLength > 0)
	{
		if (m_data->isParsedPageExists(parsedPagePtr, StorageType::AllMetaKeywordsUrlStorageType))
		{
			ParsedPagePtr duplicate = m_data->parsedPage(parsedPagePtr, StorageType::AllMetaKeywordsUrlStorageType);
			m_data->addParsedPage(parsedPagePtr, StorageType::DuplicatedMetaKeywordsUrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedMetaKeywordsUrlStorageType);
		}
		else
		{
			m_data->addParsedPage(parsedPagePtr, StorageType::AllMetaKeywordsUrlStorageType);
		}
	}
	

	if (parsedPagePtr->hasSeveralMetaKeywordsTags)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::SeveralMetaKeywordsUrlStorageType);
	}
}

void ModelController::processParsedPageH1(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->isThisExternalPage)
	{
		return;
	}

	const int h1Length = parsedPagePtr->firstH1.size();

	if (h1Length == 0)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::MissingH1UrlStorageType);
	}
	else if (h1Length > m_crawlerOptions.maxH1LengthChars)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongH1UrlStorageType);
	}

	if (h1Length > 0)
	{
		if (m_data->isParsedPageExists(parsedPagePtr, StorageType::AllH1UrlStorageType))
		{
			ParsedPagePtr duplicate = m_data->parsedPage(parsedPagePtr, StorageType::AllH1UrlStorageType);
			m_data->addParsedPage(parsedPagePtr, StorageType::DuplicatedH1UrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedH1UrlStorageType);
		}
		else
		{
			m_data->addParsedPage(parsedPagePtr, StorageType::AllH1UrlStorageType);
		}
	}

	if (parsedPagePtr->hasSeveralH1Tags)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::SeveralH1UrlStorageType);
	}

}

void ModelController::processParsedPageH2(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->isThisExternalPage)
	{
		return;
	}

	const int h2Length = parsedPagePtr->firstH2.size();

	if (h2Length == 0)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::MissingH2UrlStorageType);
	}
	else if (h2Length > m_crawlerOptions.maxH2LengthChars)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongH2UrlStorageType);
	}

	if (h2Length > 0)
	{
		if (m_data->isParsedPageExists(parsedPagePtr, StorageType::AllH2UrlStorageType))
		{
			ParsedPagePtr duplicate = m_data->parsedPage(parsedPagePtr, StorageType::AllH2UrlStorageType);
			m_data->addParsedPage(parsedPagePtr, StorageType::DuplicatedH2UrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedH2UrlStorageType);
		}
		else
		{
			m_data->addParsedPage(parsedPagePtr, StorageType::AllH2UrlStorageType);
		}
	}

	if (parsedPagePtr->hasSeveralH2Tags)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::SeveralH2UrlStorageType);
	}
}

void ModelController::processParsedPageImage(ParsedPagePtr parsedPagePtr) noexcept
{
	const int altLength = parsedPagePtr->altText.size();
	const int sizeKB = parsedPagePtr->pageSizeKilobytes;
	if (altLength > m_crawlerOptions.maxImageAltTextChars)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::VeryLongAltTextImageStorageType);
	}

	if (altLength == 0)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::MissingAltTextImageStorageType);
	}

	if (sizeKB > m_crawlerOptions.maxImageSizeKb)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::Over100kbImageStorageType);
	}
}

void ModelController::processParsedPageStatusCode(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->statusCode == 404)
	{
		m_data->addParsedPage(parsedPagePtr, StorageType::Status404StorageType);
	}
}

void ModelController::processParsedPageHtmlResources(ParsedPagePtr parsedPagePtr) noexcept
{
	if (parsedPagePtr->resourceType != ResourceType::ResourceHtml)
	{
		// if it is not an html resource, just exit
		return;
	}

	// 1. get from pending resources if exists
	ParsedPagePtr pendingPageRaw = m_data->parsedPage(parsedPagePtr, StorageType::PendingResourcesStorageType);

	// 2. if it's really html resource and pending page exists - merge two pages
	parsedPagePtr = mergeTwoPages(pendingPageRaw, parsedPagePtr);

	// 3. add this page to html resources storage
	StorageType storage = parsedPagePtr->isThisExternalPage ?
		StorageType::ExternalHtmlResourcesStorageType : StorageType::HtmlResourcesStorageType;

	m_data->addParsedPage(parsedPagePtr, storage);

	if (parsedPagePtr->isThisExternalPage)
	{
		// do not parse resources from an external one
		return;
	}

	ParsedPagePtr resourcePage = std::make_shared<ParsedPage>();
	for (const RawResourceOnPage& resource : parsedPagePtr->allResourcesOnPage)
	{
		if (resource.resourceType != ResourceType::ResourceHtml)
		{
			continue;
		}

		resourcePage->url = resource.thisResourceLink.url;
		ParsedPagePtr existingResource = m_data->parsedPage(resourcePage, StorageType::CrawledUrlStorageType);
		if (!existingResource)
		{
			existingResource = m_data->parsedPage(resourcePage, StorageType::PendingResourcesStorageType);
		}

		if (existingResource)
		{
			//assert(!m_data->isPageRawExists(resourcePage, DataCollection::HtmlPendingResourcesStorageType));
			existingResource->linksToThisPage.push_back({ parsedPagePtr, resource.thisResourceLink.urlParameter });
			parsedPagePtr->linksOnThisPage.push_back({ existingResource, resource.thisResourceLink.urlParameter });

		}
		else
		{
			ParsedPagePtr pendingResource = std::make_shared<ParsedPage>();
			pendingResource->url = resource.thisResourceLink.url;
			pendingResource->linksToThisPage.push_back({ parsedPagePtr, resource.thisResourceLink.urlParameter });
			parsedPagePtr->linksOnThisPage.push_back({ pendingResource, resource.thisResourceLink.urlParameter });
			m_data->addParsedPage(pendingResource, StorageType::PendingResourcesStorageType);
			DEBUG_ASSERT(m_data->isParsedPageExists(pendingResource, StorageType::PendingResourcesStorageType));
		}
	}
}

void ModelController::processParsedPageResources(ParsedPagePtr parsedPagePtr) noexcept
{
	std::map<ResourceType, StorageType> storageTypes
	{
		{ ResourceType::ResourceImage, StorageType::ImageResourcesStorageType },
		{ ResourceType::ResourceJavaScript, StorageType::JavaScriptResourcesStorageType },
		{ ResourceType::ResourceStyleSheet, StorageType::StyleSheetResourcesStorageType },
		{ ResourceType::ResourceFlash, StorageType::FlashResourcesStorageType },
		{ ResourceType::ResourceVideo, StorageType::VideoResourcesStorageType },
		{ ResourceType::ResourceOther, StorageType::OtherResourcesStorageType },
	};

	std::map<ResourceType, StorageType> externalStorageTypes
	{
		{ ResourceType::ResourceImage, StorageType::ExternalImageResourcesStorageType },
		{ ResourceType::ResourceJavaScript, StorageType::ExternalJavaScriptResourcesStorageType },
		{ ResourceType::ResourceStyleSheet, StorageType::ExternalStyleSheetResourcesStorageType },
		{ ResourceType::ResourceFlash, StorageType::ExternalFlashResourcesStorageType },
		{ ResourceType::ResourceVideo, StorageType::ExternalVideoResourcesStorageType },
		{ ResourceType::ResourceOther, StorageType::ExternalOtherResourcesStorageType },
	};


	const bool http = PageParserHelpers::isHttpOrHttpsScheme(parsedPagePtr->url.toDisplayString());
	const bool externalOrNotHttp = parsedPagePtr->isThisExternalPage || !http;

	if (parsedPagePtr->resourceType != ResourceType::ResourceHtml)
	{
		ParsedPagePtr pendingPageRaw = m_data->parsedPage(parsedPagePtr, StorageType::PendingResourcesStorageType);
		parsedPagePtr = mergeTwoPages(pendingPageRaw, parsedPagePtr);
		
		StorageType storage = externalOrNotHttp ?
			externalStorageTypes[parsedPagePtr->resourceType] : storageTypes[parsedPagePtr->resourceType];
		
		m_data->addParsedPage(parsedPagePtr, storage);
	}

	if (parsedPagePtr->isThisExternalPage)
	{
		// do not parse resources from an external one
		return;
	}

	for (const RawResourceOnPage& resource : parsedPagePtr->allResourcesOnPage)
	{
		QString resourceDisplayUrl = resource.thisResourceLink.url.toDisplayString();

		if (resource.resourceType == ResourceType::ResourceHtml ||
			resourceDisplayUrl.startsWith("javascript:") ||
			resourceDisplayUrl.startsWith("#"))
		{
			continue;
		}

		ParsedPagePtr resourceRaw = std::make_shared<ParsedPage>();
		resourceRaw->url = resource.thisResourceLink.url;

		bool httpResource = PageParserHelpers::isHttpOrHttpsScheme(resourceDisplayUrl);
		bool externalOrNotHttpResource = PageParserHelpers::isUrlExternal(parsedPagePtr->url, resourceRaw->url) || !httpResource;

		StorageType storage = externalOrNotHttpResource ?
			externalStorageTypes[resource.resourceType] : storageTypes[resource.resourceType];

		ParsedPagePtr newOrExistingResource = m_data->parsedPage(resourceRaw, storage);
		if (!newOrExistingResource)
		{
			newOrExistingResource = m_data->parsedPage(resourceRaw, StorageType::PendingResourcesStorageType);
		}

		if (!newOrExistingResource)
		{
			newOrExistingResource = resourceRaw;
			
			m_data->addParsedPage(newOrExistingResource, 
				httpResource ? StorageType::PendingResourcesStorageType : storage);
		}

		parsedPagePtr->linksOnThisPage.push_back({ newOrExistingResource, resource.thisResourceLink.urlParameter });
		newOrExistingResource->linksToThisPage.push_back({ parsedPagePtr, resource.thisResourceLink.urlParameter });
		newOrExistingResource->resourceType = resource.resourceType;
	}
}

void ModelController::fixParsedPageResourceType(ParsedPagePtr parsedPagePtr) noexcept
{
	ParsedPagePtr pendingResource = m_data->parsedPage(parsedPagePtr, StorageType::PendingResourcesStorageType);
	if (pendingResource && pendingResource->resourceType != ResourceType::ResourceHtml)
	{
		parsedPagePtr->resourceType = pendingResource->resourceType;
	}
}

}