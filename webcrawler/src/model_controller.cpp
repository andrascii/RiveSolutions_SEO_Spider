#include "model_controller.h"
#include "data_collection.h"

namespace WebCrawler
{

ModelController::ModelController(QObject* parent)
	: QObject(parent)
	, m_data(new DataCollection(this))
{
}

void ModelController::setWebCrawlerOptions(const WebCrawlerOptions& options)
{
	m_webCrawlerOptions = options;
}

void ModelController::addPageRaw(PageRawPtr pageRaw) noexcept
{
	processPageRawHtmlResources(pageRaw);
	pageRaw->rawResources.clear();

	processPageRawStatusCode(pageRaw);

	if (pageRaw->fromUrl.toString().isEmpty())
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

	if (h1 == title)
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
	const int altLength = pageRaw->imageAltText.size();
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
	// 1. remove from pending resources if exists
	PageRawPtr pendingPageRaw = m_data->removePageRaw(pageRaw, DataCollection::HtmlPendingResourcesStorageType);

	if (pageRaw->resourceType != PageRawResource::ResourceHtml)
	{
		// if it is not an html resource, just remove it from pending resources and exit
		return;
	}

	// 2. if it's really html resource and pending page exists - merge two pages
	if (pendingPageRaw)
	{
		std::copy(std::begin(pendingPageRaw->linksToThisResource),
			std::end(pendingPageRaw->linksToThisResource),
			std::begin(pageRaw->linksToThisResource));

		*pendingPageRaw = *pageRaw;
		// we should use the original PageRawPtr because there are pages containing links to it
		pageRaw = pendingPageRaw;
	}

	// 3. add this page to html resources storage
	m_data->addPageRaw(pageRaw, DataCollection::HtmlResourcesStorageType);

	PageRawPtr resourcePage = std::make_shared<PageRaw>();
	for (const PageRawResource& resource : pageRaw->rawResources)
	{
		if (resource.resourceType != PageRawResource::ResourceHtml)
		{
			continue;
		}

		resourcePage->url = resource.resourceUrl;
		INFOLOG << "Trying to find " << resourcePage->url.toDisplayString();
		PageRawPtr existingResource = m_data->pageRaw(resourcePage, DataCollection::CrawledUrlStorageType);
		if (!existingResource)
		{
			existingResource = m_data->pageRaw(resourcePage, DataCollection::HtmlPendingResourcesStorageType);
		}

		if (existingResource)
		{
			//assert(!m_data->isPageRawExists(resourcePage, DataCollection::HtmlPendingResourcesStorageType));
			existingResource->linksToThisResource.push_back(pageRaw);
			pageRaw->linksFromThisResource.push_back(existingResource);

		}
		else
		{
			PageRawPtr pendingResource = std::make_shared<PageRaw>();
			pendingResource->url = resource.resourceUrl;
			pendingResource->linksToThisResource.push_back(pageRaw);
			pageRaw->linksFromThisResource.push_back(pendingResource);
			m_data->addPageRaw(pendingResource, DataCollection::HtmlPendingResourcesStorageType);
			assert(m_data->isPageRawExists(pendingResource, DataCollection::HtmlPendingResourcesStorageType));

			// isPageRawExists is not working correctly, uncomment this code to check it
// 			PageRawPtr pendingResourceCopy = std::make_shared<PageRaw>();
// 			pendingResourceCopy->url = resource.resourceUrl;
// 			assert(m_data->isPageRawExists(pendingResourceCopy, DataCollection::HtmlPendingResourcesStorageType));
		}
	}
}

void ModelController::processPageRawResources(PageRawPtr pageRaw) noexcept
{
	std::map<PageRawResource::ResourceType, DataCollection::StorageType> storageTypes
	{
		{ PageRawResource::ResourceImage, DataCollection::ImageResourcesStorageType },
		{ PageRawResource::ResourceJavaScript, DataCollection::JavaScriptResourcesStorageType },
		{ PageRawResource::ResourceStyleSheet, DataCollection::StyleSheetResourcesStorageType },
		{ PageRawResource::ResourceFlash, DataCollection::FlashResourcesStorageType },
		{ PageRawResource::ResourceVideo, DataCollection::VideoResourcesStorageType },
		{ PageRawResource::ResourceOther, DataCollection::OtherResourcesStorageType },
	};

	for (const PageRawResource& resource : pageRaw->rawResources)
	{
		if (resource.resourceType == PageRawResource::ResourceHtml)
		{
			continue;
		}

		PageRawPtr newResource = std::make_shared<PageRaw>();
		newResource->linksToThisResource.push_back(pageRaw);
		newResource->url = resource.resourceUrl;
		newResource->resourceType = resource.resourceType;
	}
}

}