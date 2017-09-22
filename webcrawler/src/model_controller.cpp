#include "model_controller.h"
#include "data_collection.h"
#include "page_raw_parser_helpers.h"

namespace
{

WebCrawler::PageRawPtr mergeTwoPages(WebCrawler::PageRawPtr existingPage, WebCrawler::PageRawPtr newPage)
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
	// we should use the original PageRawPtr because there are pages containing links to it
	return existingPage;
}

}

namespace WebCrawler
{

ModelController::ModelController(QObject* parent)
	: QObject(parent)
	, m_data(new DataCollection(this))
{
}

ModelController::~ModelController()
{
	
}

void ModelController::setWebCrawlerOptions(const WebCrawlerOptions& options)
{
	m_webCrawlerOptions = options;
}

void ModelController::addPageRaw(PageRawPtr pageRaw) noexcept
{
	ASSERT(pageRaw->resourceType >= ResourceType::ResourceHtml &&
		pageRaw->resourceType <= ResourceType::ResourceOther)

	fixPageRawResourceType(pageRaw);
	processPageRawHtmlResources(pageRaw);
	processPageRawResources(pageRaw);
	pageRaw->allResourcesOnPage.clear();

	processPageRawStatusCode(pageRaw);

	if (pageRaw->resourceType == ResourceType::ResourceHtml)
	{
		// page
		processPageRawUrl(pageRaw);
		processPageRawTitle(pageRaw);
		processPageRawMetaDescription(pageRaw);
		processPageRawMetaKeywords(pageRaw);
		processPageRawH1(pageRaw);
		processPageRawH2(pageRaw);
	}
	else
	{
		// image resource
		processPageRawImage(pageRaw);
	}

	m_data->removePageRaw(pageRaw, DataCollection::PendingResourcesStorageType);
}

const DataCollection* ModelController::data() const noexcept
{
	return m_data;
}

DataCollection* ModelController::data() noexcept
{
	return m_data;
}

void ModelController::processPageRawUrl(PageRawPtr pageRaw) noexcept
{
	const QUrl url = pageRaw->url;
	const QString urlStr = url.toString();

	m_data->addPageRaw(pageRaw, DataCollection::CrawledUrlStorageType);

	if (url.host() != m_webCrawlerOptions.url.host())
	{
		m_data->addPageRaw(pageRaw, DataCollection::ExternalUrlStorageType);
	}

	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	if (urlStr.toLower() != urlStr)
	{
		m_data->addPageRaw(pageRaw, DataCollection::UpperCaseUrlStorageType);
	}

	if (urlStr.size() > m_webCrawlerOptions.limitMaxUrlLength)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongUrlStorageType);
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
		m_data->addPageRaw(pageRaw, DataCollection::NonAsciiCharacterUrlStorageType);
	}
}

void ModelController::processPageRawTitle(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	int statusCode = pageRaw->statusCode;
	const QString title = pageRaw->title;
	const QString h1 = pageRaw->firstH1;

	//
	// Replace 301 and 302 constants
	//
	if (title.isEmpty() ||
		statusCode == 301 || statusCode == 302)
	{
		m_data->addPageRaw(pageRaw, DataCollection::EmptyTitleUrlStorageType);
	}
	else if (title.size() > m_webCrawlerOptions.maxTitleLength)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongTitleUrlStorageType);
	}
	else if (title.size() < m_webCrawlerOptions.minTitleLength)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongTitleUrlStorageType);
	}

	if (!title.isEmpty())
	{
		if (m_data->isPageRawExists(pageRaw, DataCollection::AllTitlesUrlStorageType))
		{
			PageRawPtr duplicate = m_data->pageRaw(pageRaw, DataCollection::AllTitlesUrlStorageType);
			m_data->addPageRaw(pageRaw, DataCollection::DuplicatedTitleUrlStorageType);
			m_data->addPageRaw(duplicate, DataCollection::DuplicatedTitleUrlStorageType);
		}
		else
		{
			m_data->addPageRaw(pageRaw, DataCollection::AllTitlesUrlStorageType);
		}
	}

	if (!h1.isEmpty() && h1 == title)
	{
		m_data->addPageRaw(pageRaw, DataCollection::DuplicatedH1TitleUrlStorageType);
	}

	if (pageRaw->hasSeveralTitleTags)
	{
		m_data->addPageRaw(pageRaw, DataCollection::SeveralTitleUrlStorageType);
	}
}

void ModelController::processPageRawMetaDescription(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	const int metaDescriptionLength = pageRaw->metaDescription.size();

	if (metaDescriptionLength == 0)
	{
		m_data->addPageRaw(pageRaw, DataCollection::EmptyMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength > m_webCrawlerOptions.maxDescriptionLength)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < m_webCrawlerOptions.minDescriptionLength)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0)
	{
		if (m_data->isPageRawExists(pageRaw, DataCollection::AllMetaDescriptionsUrlStorageType))
		{
			PageRawPtr duplicate = m_data->pageRaw(pageRaw, DataCollection::AllMetaDescriptionsUrlStorageType);
			m_data->addPageRaw(pageRaw, DataCollection::DuplicatedMetaDescriptionUrlStorageType);
			m_data->addPageRaw(duplicate, DataCollection::DuplicatedMetaDescriptionUrlStorageType);
		}
		else
		{
			m_data->addPageRaw(pageRaw, DataCollection::AllMetaDescriptionsUrlStorageType);
		}
	}

	if (pageRaw->hasSeveralMetaDescriptionTags)
	{
		m_data->addPageRaw(pageRaw, DataCollection::SeveralMetaDescriptionUrlStorageType);
	}
}

void ModelController::processPageRawMetaKeywords(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	const int metaKeywordsLength = pageRaw->metaKeywords.size();

	if (metaKeywordsLength == 0)
	{
		m_data->addPageRaw(pageRaw, DataCollection::EmptyMetaKeywordsUrlStorageType);
	}

	if (metaKeywordsLength > 0)
	{
		if (m_data->isPageRawExists(pageRaw, DataCollection::AllMetaKeywordsUrlStorageType))
		{
			PageRawPtr duplicate = m_data->pageRaw(pageRaw, DataCollection::AllMetaKeywordsUrlStorageType);
			m_data->addPageRaw(pageRaw, DataCollection::DuplicatedMetaKeywordsUrlStorageType);
			m_data->addPageRaw(duplicate, DataCollection::DuplicatedMetaKeywordsUrlStorageType);
		}
		else
		{
			m_data->addPageRaw(pageRaw, DataCollection::AllMetaKeywordsUrlStorageType);
		}
	}
	

	if (pageRaw->hasSeveralMetaKeywordsTags)
	{
		m_data->addPageRaw(pageRaw, DataCollection::SeveralMetaKeywordsUrlStorageType);
	}
}

void ModelController::processPageRawH1(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	const int h1Length = pageRaw->firstH1.size();

	if (h1Length == 0)
	{
		m_data->addPageRaw(pageRaw, DataCollection::MissingH1UrlStorageType);
	}
	else if (h1Length > m_webCrawlerOptions.maxH1LengthChars)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongH1UrlStorageType);
	}

	if (h1Length > 0)
	{
		if (m_data->isPageRawExists(pageRaw, DataCollection::AllH1UrlStorageType))
		{
			PageRawPtr duplicate = m_data->pageRaw(pageRaw, DataCollection::AllH1UrlStorageType);
			m_data->addPageRaw(pageRaw, DataCollection::DuplicatedH1UrlStorageType);
			m_data->addPageRaw(duplicate, DataCollection::DuplicatedH1UrlStorageType);
		}
		else
		{
			m_data->addPageRaw(pageRaw, DataCollection::AllH1UrlStorageType);
		}
	}

	if (pageRaw->hasSeveralH1Tags)
	{
		m_data->addPageRaw(pageRaw, DataCollection::SeveralH1UrlStorageType);
	}

}

void ModelController::processPageRawH2(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	const int h2Length = pageRaw->firstH2.size();

	if (h2Length == 0)
	{
		m_data->addPageRaw(pageRaw, DataCollection::MissingH2UrlStorageType);
	}
	else if (h2Length > m_webCrawlerOptions.maxH2LengthChars)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongH2UrlStorageType);
	}

	if (h2Length > 0)
	{
		if (m_data->isPageRawExists(pageRaw, DataCollection::AllH2UrlStorageType))
		{
			PageRawPtr duplicate = m_data->pageRaw(pageRaw, DataCollection::AllH2UrlStorageType);
			m_data->addPageRaw(pageRaw, DataCollection::DuplicatedH2UrlStorageType);
			m_data->addPageRaw(duplicate, DataCollection::DuplicatedH2UrlStorageType);
		}
		else
		{
			m_data->addPageRaw(pageRaw, DataCollection::AllH2UrlStorageType);
		}
	}

	if (pageRaw->hasSeveralH2Tags)
	{
		m_data->addPageRaw(pageRaw, DataCollection::SeveralH2UrlStorageType);
	}
}

void ModelController::processPageRawImage(PageRawPtr pageRaw) noexcept
{
	const int altLength = pageRaw->altText.size();
	const int sizeKB = pageRaw->pageSizeKb;
	if (altLength > m_webCrawlerOptions.maxImageAltTextChars)
	{
		m_data->addPageRaw(pageRaw, DataCollection::VeryLongAltTextImageStorageType);
	}

	if (altLength == 0)
	{
		m_data->addPageRaw(pageRaw, DataCollection::MissingAltTextImageStorageType);
	}

	if (sizeKB > m_webCrawlerOptions.maxImageSizeKb)
	{
		m_data->addPageRaw(pageRaw, DataCollection::Over100kbImageStorageType);
	}
}

void ModelController::processPageRawStatusCode(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->statusCode == 404)
	{
		m_data->addPageRaw(pageRaw, DataCollection::Status404StorageType);
	}
}

void ModelController::processPageRawHtmlResources(PageRawPtr pageRaw) noexcept
{
	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		// if it is not an html resource, just exit
		return;
	}

	// 1. get from pending resources if exists
	PageRawPtr pendingPageRaw = m_data->pageRaw(pageRaw, DataCollection::PendingResourcesStorageType);

	// 2. if it's really html resource and pending page exists - merge two pages
	pageRaw = mergeTwoPages(pendingPageRaw, pageRaw);

	// 3. add this page to html resources storage
	DataCollection::StorageType storage = pageRaw->isThisExternalPage ?
		DataCollection::ExternalHtmlResourcesStorageType : DataCollection::HtmlResourcesStorageType;

	m_data->addPageRaw(pageRaw, storage);

	if (pageRaw->isThisExternalPage)
	{
		// do not parse resources from an external one
		return;
	}

	PageRawPtr resourcePage = std::make_shared<PageRaw>();
	for (const RawResourceOnPage& resource : pageRaw->allResourcesOnPage)
	{
		if (resource.resourceType != ResourceType::ResourceHtml)
		{
			continue;
		}

		resourcePage->url = resource.thisResourceUrl.url;
		PageRawPtr existingResource = m_data->pageRaw(resourcePage, DataCollection::CrawledUrlStorageType);
		if (!existingResource)
		{
			existingResource = m_data->pageRaw(resourcePage, DataCollection::PendingResourcesStorageType);
		}

		if (existingResource)
		{
			//assert(!m_data->isPageRawExists(resourcePage, DataCollection::HtmlPendingResourcesStorageType));
			existingResource->linksToThisPage.push_back({ pageRaw, resource.thisResourceUrl.urlParameter });
			pageRaw->linksOnThisPage.push_back({ existingResource, resource.thisResourceUrl.urlParameter });

		}
		else
		{
			PageRawPtr pendingResource = std::make_shared<PageRaw>();
			pendingResource->url = resource.thisResourceUrl.url;
			pendingResource->linksToThisPage.push_back({ pageRaw, resource.thisResourceUrl.urlParameter });
			pageRaw->linksOnThisPage.push_back({ pendingResource, resource.thisResourceUrl.urlParameter });
			m_data->addPageRaw(pendingResource, DataCollection::PendingResourcesStorageType);
			DEBUG_ASSERT(m_data->isPageRawExists(pendingResource, DataCollection::PendingResourcesStorageType));
		}
	}
}

void ModelController::processPageRawResources(PageRawPtr pageRaw) noexcept
{
	std::map<ResourceType, DataCollection::StorageType> storageTypes
	{
		{ ResourceType::ResourceImage, DataCollection::ImageResourcesStorageType },
		{ ResourceType::ResourceJavaScript, DataCollection::JavaScriptResourcesStorageType },
		{ ResourceType::ResourceStyleSheet, DataCollection::StyleSheetResourcesStorageType },
		{ ResourceType::ResourceFlash, DataCollection::FlashResourcesStorageType },
		{ ResourceType::ResourceVideo, DataCollection::VideoResourcesStorageType },
		{ ResourceType::ResourceOther, DataCollection::OtherResourcesStorageType },
	};

	std::map<ResourceType, DataCollection::StorageType> externalStorageTypes
	{
		{ ResourceType::ResourceImage, DataCollection::ExternalImageResourcesStorageType },
		{ ResourceType::ResourceJavaScript, DataCollection::ExternalJavaScriptResourcesStorageType },
		{ ResourceType::ResourceStyleSheet, DataCollection::ExternalStyleSheetResourcesStorageType },
		{ ResourceType::ResourceFlash, DataCollection::ExternalFlashResourcesStorageType },
		{ ResourceType::ResourceVideo, DataCollection::ExternalVideoResourcesStorageType },
		{ ResourceType::ResourceOther, DataCollection::ExternalOtherResourcesStorageType },
	};


	const bool http = PageRawParserHelpers::isHttpOrHttpsScheme(pageRaw->url.toDisplayString());
	const bool externalOrNotHttp = pageRaw->isThisExternalPage || !http;

	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		PageRawPtr pendingPageRaw = m_data->pageRaw(pageRaw, DataCollection::PendingResourcesStorageType);
		pageRaw = mergeTwoPages(pendingPageRaw, pageRaw);
		
		DataCollection::StorageType storage = externalOrNotHttp ?
			externalStorageTypes[pageRaw->resourceType] : storageTypes[pageRaw->resourceType];
		
		m_data->addPageRaw(pageRaw, storage);
	}

	if (pageRaw->isThisExternalPage)
	{
		// do not parse resources from an external one
		return;
	}

	for (const RawResourceOnPage& resource : pageRaw->allResourcesOnPage)
	{
		QString resourceDisplayUrl = resource.thisResourceUrl.url.toDisplayString();

		if (resource.resourceType == ResourceType::ResourceHtml ||
			resourceDisplayUrl.startsWith("javascript:") ||
			resourceDisplayUrl.startsWith("#"))
		{
			continue;
		}

		PageRawPtr resourceRaw = std::make_shared<PageRaw>();
		resourceRaw->url = resource.thisResourceUrl.url;

		bool httpResource = PageRawParserHelpers::isHttpOrHttpsScheme(resourceDisplayUrl);
		bool externalOrNotHttpResource = PageRawParserHelpers::isUrlExternal(pageRaw->url, resourceRaw->url) || !httpResource;

		DataCollection::StorageType storage = externalOrNotHttpResource ?
			externalStorageTypes[resource.resourceType] : storageTypes[resource.resourceType];

		PageRawPtr newOrExistingResource = m_data->pageRaw(resourceRaw, storage);
		if (!newOrExistingResource)
		{
			newOrExistingResource = m_data->pageRaw(resourceRaw, DataCollection::PendingResourcesStorageType);
		}

		if (!newOrExistingResource)
		{
			newOrExistingResource = resourceRaw;
			
			m_data->addPageRaw(newOrExistingResource, 
				httpResource ? DataCollection::PendingResourcesStorageType : storage);
		}

		pageRaw->linksOnThisPage.push_back({ newOrExistingResource, resource.thisResourceUrl.urlParameter });
		newOrExistingResource->linksToThisPage.push_back({ pageRaw, resource.thisResourceUrl.urlParameter });
		newOrExistingResource->resourceType = resource.resourceType;
	}
}

void ModelController::fixPageRawResourceType(PageRawPtr pageRaw) noexcept
{
	PageRawPtr pendingResource = m_data->pageRaw(pageRaw, DataCollection::PendingResourcesStorageType);
	if (pendingResource && pendingResource->resourceType != ResourceType::ResourceHtml)
	{
		pageRaw->resourceType = pendingResource->resourceType;
	}
}

}