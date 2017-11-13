#include "model_controller.h"
#include "unordered_data_collection.h"
#include "page_parser_helpers.h"

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
}

void ModelController::addParsedPage(ParsedPagePtr incomingPage) noexcept
{
	ASSERT(incomingPage->resourceType >= ResourceType::ResourceHtml &&
		incomingPage->resourceType <= ResourceType::ResourceOther);

	fixParsedPageResourceType(incomingPage);

	if (!resourceShouldBeProcessed(incomingPage->resourceType))
	{
		m_data->removeParsedPage(incomingPage, StorageType::PendingResourcesStorageType);
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

	m_data->removeParsedPage(incomingPage, StorageType::PendingResourcesStorageType);

	if (!m_linksToPageChanges.changes.empty())
	{
		m_data->parsedPageLinksToThisResourceChanged(m_linksToPageChanges);
		m_linksToPageChanges.changes.clear();
	}
}

const UnorderedDataCollection* ModelController::data() const noexcept
{
	return m_data;
}

UnorderedDataCollection* ModelController::data() noexcept
{
	return m_data;
}

void ModelController::processParsedPageUrl(ParsedPagePtr& incomingPage) const noexcept
{
	const QUrl url = incomingPage->url;
	const QString urlStr = url.toString();

	m_data->addParsedPage(incomingPage, StorageType::CrawledUrlStorageType);

	if (url.host() != m_crawlerOptions.host.host())
	{
		m_data->addParsedPage(incomingPage, StorageType::ExternalUrlStorageType);
	}

	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	if (urlStr.toLower() != urlStr)
	{
		m_data->addParsedPage(incomingPage, StorageType::UpperCaseUrlStorageType);
	}

	if (urlStr.size() > m_crawlerOptions.limitMaxUrlLength)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryLongUrlStorageType);
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
		m_data->addParsedPage(incomingPage, StorageType::NonAsciiCharacterUrlStorageType);
	}
}

void ModelController::processParsedPageTitle(ParsedPagePtr& incomingPage) const noexcept
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
		m_data->addParsedPage(incomingPage, StorageType::EmptyTitleUrlStorageType);
	}
	else if (title.size() > m_crawlerOptions.maxTitleLength && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryLongTitleUrlStorageType);
	}
	else if (title.size() < m_crawlerOptions.minTitleLength && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryShortTitleUrlStorageType);
	}

	if (!title.isEmpty() && successfulResponseCode)
	{
		if (m_data->isParsedPageExists(incomingPage, StorageType::AllTitlesUrlStorageType))
		{
			const ParsedPagePtr duplicate = m_data->parsedPage(incomingPage, StorageType::AllTitlesUrlStorageType);
			m_data->addParsedPage(incomingPage, StorageType::DuplicatedTitleUrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedTitleUrlStorageType);
		}
		else
		{
			m_data->addParsedPage(incomingPage, StorageType::AllTitlesUrlStorageType);
		}
	}

	if (!h1.isEmpty() && h1 == title && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::DuplicatedH1TitleUrlStorageType);
	}

	if (incomingPage->hasSeveralTitleTags && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::SeveralTitleUrlStorageType);
	}
}

void ModelController::processParsedPageMetaDescription(ParsedPagePtr& incomingPage) const noexcept
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	const int metaDescriptionLength = incomingPage->metaDescription.size();

	if (metaDescriptionLength == 0 && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::EmptyMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength > m_crawlerOptions.maxDescriptionLength && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < m_crawlerOptions.minDescriptionLength && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0 && successfulResponseCode)
	{
		if (m_data->isParsedPageExists(incomingPage, StorageType::AllMetaDescriptionsUrlStorageType))
		{
			const ParsedPagePtr duplicate = m_data->parsedPage(incomingPage, StorageType::AllMetaDescriptionsUrlStorageType);
			m_data->addParsedPage(incomingPage, StorageType::DuplicatedMetaDescriptionUrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedMetaDescriptionUrlStorageType);
		}
		else
		{
			m_data->addParsedPage(incomingPage, StorageType::AllMetaDescriptionsUrlStorageType);
		}
	}

	if (incomingPage->hasSeveralMetaDescriptionTags && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::SeveralMetaDescriptionUrlStorageType);
	}
}

void ModelController::processParsedPageMetaKeywords(ParsedPagePtr& incomingPage) const noexcept
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	const int metaKeywordsLength = incomingPage->metaKeywords.size();

	if (metaKeywordsLength == 0 && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::EmptyMetaKeywordsUrlStorageType);
	}

	if (metaKeywordsLength > 0 && successfulResponseCode)
	{
		if (m_data->isParsedPageExists(incomingPage, StorageType::AllMetaKeywordsUrlStorageType))
		{
			ParsedPagePtr duplicate = m_data->parsedPage(incomingPage, StorageType::AllMetaKeywordsUrlStorageType);
			m_data->addParsedPage(incomingPage, StorageType::DuplicatedMetaKeywordsUrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedMetaKeywordsUrlStorageType);
		}
		else
		{
			m_data->addParsedPage(incomingPage, StorageType::AllMetaKeywordsUrlStorageType);
		}
	}
	

	if (incomingPage->hasSeveralMetaKeywordsTags && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::SeveralMetaKeywordsUrlStorageType);
	}
}

void ModelController::processParsedPageH1(ParsedPagePtr& incomingPage) const noexcept
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	const int h1Length = incomingPage->firstH1.size();

	if (h1Length == 0 && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::MissingH1UrlStorageType);
	}
	else if (h1Length > m_crawlerOptions.maxH1LengthChars && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryLongH1UrlStorageType);
	}

	if (h1Length > 0 && successfulResponseCode)
	{
		if (m_data->isParsedPageExists(incomingPage, StorageType::AllH1UrlStorageType))
		{
			const ParsedPagePtr duplicate = m_data->parsedPage(incomingPage, StorageType::AllH1UrlStorageType);
			m_data->addParsedPage(incomingPage, StorageType::DuplicatedH1UrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedH1UrlStorageType);
		}
		else
		{
			m_data->addParsedPage(incomingPage, StorageType::AllH1UrlStorageType);
		}
	}

	if (incomingPage->hasSeveralH1Tags && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::SeveralH1UrlStorageType);
	}

}

void ModelController::processParsedPageH2(ParsedPagePtr& incomingPage) const noexcept
{
	if (incomingPage->isThisExternalPage)
	{
		return;
	}

	const bool successfulResponseCode = successfulCode(incomingPage);

	const int h2Length = incomingPage->firstH2.size();

	if (h2Length == 0 && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::MissingH2UrlStorageType);
	}
	else if (h2Length > m_crawlerOptions.maxH2LengthChars && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::VeryLongH2UrlStorageType);
	}

	if (h2Length > 0 && successfulResponseCode)
	{
		if (m_data->isParsedPageExists(incomingPage, StorageType::AllH2UrlStorageType))
		{
			const ParsedPagePtr duplicate = m_data->parsedPage(incomingPage, StorageType::AllH2UrlStorageType);
			m_data->addParsedPage(incomingPage, StorageType::DuplicatedH2UrlStorageType);
			m_data->addParsedPage(duplicate, StorageType::DuplicatedH2UrlStorageType);
		}
		else
		{
			m_data->addParsedPage(incomingPage, StorageType::AllH2UrlStorageType);
		}
	}

	if (incomingPage->hasSeveralEqualH2Tags && successfulResponseCode)
	{
		m_data->addParsedPage(incomingPage, StorageType::SeveralH2UrlStorageType);
	}
}

void ModelController::processParsedPageImage(ParsedPagePtr& incomingPage, bool checkOnlyLastResource) const noexcept
{
	const int sizeKB = incomingPage->pageSizeKilobytes;

	if (sizeKB > m_crawlerOptions.maxImageSizeKb)
	{
		m_data->addParsedPage(incomingPage, StorageType::Over100kbImageStorageType);
	}

	size_t index = 0;
	const size_t lastIndex = incomingPage->linksToThisPage.size() - 1;

	for (const ResourceLink& linkToThisImage : incomingPage->linksToThisPage)
	{
		if (checkOnlyLastResource && index != lastIndex)
		{
			// TODO: optimize
			++index;
			continue;
		}

		if (linkToThisImage.resourceSource == ResourceSource::SourceTagImg)
		{
			const int altLength = linkToThisImage.altOrTitle.size();

			if (altLength > m_crawlerOptions.maxImageAltTextChars)
			{
				m_data->addParsedPage(incomingPage, StorageType::VeryLongAltTextImageStorageType);
				incomingPage->tooLongAltIndices.push_back(index);
			}

			if (altLength == 0)
			{
				m_data->addParsedPage(incomingPage, StorageType::MissingAltTextImageStorageType);
				incomingPage->missignAltIndices.push_back(index);
			}
		}

		++index;
	}
}

void ModelController::processParsedPageStatusCode(ParsedPagePtr& incomingPage) const noexcept
{
	if (incomingPage->statusCode == Common::StatusCode::NotFound404)
	{
		m_data->addParsedPage(incomingPage, StorageType::Status404StorageType);
	}
}

void ModelController::processParsedPageHtmlResources(ParsedPagePtr& incomingPage) noexcept
{
	if (incomingPage->resourceType != ResourceType::ResourceHtml)
	{
		// if it is not an html resource, just exit
		return;
	}

	// 1. get from pending resources if exists
	const ParsedPagePtr pendingPageRaw = m_data->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);

	// 2. if it's really html resource and pending page exists - merge two pages
	incomingPage = mergeTwoPages(pendingPageRaw, incomingPage);

	// 3. add this page to html resources storage
	const StorageType storage = incomingPage->isThisExternalPage ?
		StorageType::ExternalHtmlResourcesStorageType : StorageType::HtmlResourcesStorageType;

	m_data->addParsedPage(incomingPage, storage);

	if (incomingPage->isThisExternalPage)
	{
		// do not parse resources from an external one
		return;
	}

	ParsedPagePtr resourcePage = std::make_shared<ParsedPage>();
	for (const RawResourceOnPage& resource : incomingPage->allResourcesOnPage)
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
			existingResource->linksToThisPage.push_back(
			{ 
				incomingPage, 
				resource.thisResourceLink.urlParameter, 
				resource.thisResourceLink.resourceSource,
				resource.thisResourceLink.altOrTitle 
			});
			
			m_linksToPageChanges.changes.push_back({ existingResource, existingResource->linksToThisPage.size() - 1 });
			
			incomingPage->linksOnThisPage.push_back(
			{ 
				existingResource, 
				resource.thisResourceLink.urlParameter, 
				resource.thisResourceLink.resourceSource,
				resource.thisResourceLink.altOrTitle 
			});
		}
		else
		{
			ParsedPagePtr pendingResource = std::make_shared<ParsedPage>();
			pendingResource->url = resource.thisResourceLink.url;
			pendingResource->linksToThisPage.push_back(
			{ 
				incomingPage, 
				resource.thisResourceLink.urlParameter, 
				resource.thisResourceLink.resourceSource,
				resource.thisResourceLink.altOrTitle 
			});
			
			m_linksToPageChanges.changes.push_back({ pendingResource, pendingResource->linksToThisPage.size() - 1 }); // do not do it for pending resource?
			
			incomingPage->linksOnThisPage.push_back(
			{ 
				pendingResource, 
				resource.thisResourceLink.urlParameter, 
				resource.thisResourceLink.resourceSource,
				resource.thisResourceLink.altOrTitle 
			});
			
			m_data->addParsedPage(pendingResource, StorageType::PendingResourcesStorageType);
			DEBUG_ASSERT(m_data->isParsedPageExists(pendingResource, StorageType::PendingResourcesStorageType));
		}
	}
}

void ModelController::processParsedPageResources(ParsedPagePtr& incomingPage) noexcept
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


	const bool http = PageParserHelpers::isHttpOrHttpsScheme(incomingPage->url);
	const bool externalOrNotHttp = incomingPage->isThisExternalPage || !http;

	if (incomingPage->resourceType != ResourceType::ResourceHtml)
	{
		const ParsedPagePtr pendingPageRaw = m_data->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
		incomingPage = mergeTwoPages(pendingPageRaw, incomingPage);
		
		StorageType storage = externalOrNotHttp ?
			externalStorageTypes[incomingPage->resourceType] : storageTypes[incomingPage->resourceType];
		
		m_data->addParsedPage(incomingPage, storage);
	}

	if (incomingPage->isThisExternalPage)
	{
		// do not parse resources from an external one
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

		ParsedPagePtr resourceRaw = std::make_shared<ParsedPage>();
		resourceRaw->url = resource.thisResourceLink.url;

		const bool httpResource = PageParserHelpers::isHttpOrHttpsScheme(resource.thisResourceLink.url);
		const bool externalOrNotHttpResource = PageParserHelpers::isUrlExternal(incomingPage->url, resourceRaw->url) || !httpResource;

		const StorageType storage = externalOrNotHttpResource ?
			externalStorageTypes[resource.resourceType] : storageTypes[resource.resourceType];

		ParsedPagePtr newOrExistingResource = m_data->parsedPage(resourceRaw, storage);
		
		const bool existingImageResource = newOrExistingResource && 
			newOrExistingResource->resourceType == ResourceType::ResourceImage &&
			resource.thisResourceLink.resourceSource == ResourceSource::SourceTagImg;

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

		incomingPage->linksOnThisPage.push_back(
		{ 
			newOrExistingResource, 
			resource.thisResourceLink.urlParameter, 
			resource.thisResourceLink.resourceSource, 
			resource.thisResourceLink.altOrTitle 
		});
		
		newOrExistingResource->linksToThisPage.push_back(
		{ 
			incomingPage, 
			resource.thisResourceLink.urlParameter, 
			resource.thisResourceLink.resourceSource,
			resource.thisResourceLink.altOrTitle 
		});
		
		m_linksToPageChanges.changes.push_back({ newOrExistingResource, newOrExistingResource->linksToThisPage.size() - 1 });
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
	ParsedPagePtr pendingResource = m_data->parsedPage(incomingPage, StorageType::PendingResourcesStorageType);
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
		return true;
	case ResourceType::ResourceImage:
		return m_crawlerOptions.parserTypeFlags.testFlag(ImagesResourcesParserType);
	case ResourceType::ResourceJavaScript:
		return m_crawlerOptions.parserTypeFlags.testFlag(JavaScriptResourcesParserType);
	case ResourceType::ResourceStyleSheet:
		return m_crawlerOptions.parserTypeFlags.testFlag(CssResourcesParserType);
	case ResourceType::ResourceFlash:
		return m_crawlerOptions.parserTypeFlags.testFlag(FlashResourcesParserType);
	case ResourceType::ResourceVideo:
		return m_crawlerOptions.parserTypeFlags.testFlag(VideoResourcesParserType);
	case ResourceType::ResourceOther:
		return m_crawlerOptions.parserTypeFlags.testFlag(OtherResourcesParserType);
	default:
		ASSERT(!"Unexpected resource type");
		break;
	}

	return false;
}

}