//#include "application.h"
//#include "application_properties.h"
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
	if (pageRaw->url.toString().isEmpty())
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
	const QUrl url = pageRaw->url;;
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

}