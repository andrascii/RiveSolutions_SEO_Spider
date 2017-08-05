#include "application.h"
#include "application_properties.h"
#include "model_controller.h"
#include "data_collection.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: m_data(new DataCollection(this))
{
}

void ModelController::addPageInfo(PageInfoPtr pageInfo) noexcept
{
	processPageInfoUrl(pageInfo);
	processPageInfoTitle(pageInfo);
	processPageInfoMetaDescription(pageInfo);
	processPageInfoMetaKeywords(pageInfo);
	processPageInfoH1(pageInfo);
	processPageInfoH2(pageInfo);
	processPageInfoImage(pageInfo);
}

const DataCollection* ModelController::data() const noexcept
{
	return m_data;
}

DataCollection* ModelController::data() noexcept
{
	return m_data;
}

void ModelController::processPageInfoUrl(PageInfoPtr pageInfo) noexcept
{
	const QUrl url = pageInfo->itemValue(PageInfo::UrlItemType).toUrl();
	const QString urlStr = url.toString();

	m_data->addPageInfo(pageInfo, DataCollection::CrawledUrlStorageType);

	if (url.host() != theApp->properties()->url().host())
	{
		m_data->addPageInfo(pageInfo, DataCollection::ExternalUrlStorageType);
	}

	if (urlStr.toUpper() == urlStr)
	{
		m_data->addPageInfo(pageInfo, DataCollection::UpperCaseUrlStorageType);
	}

	if (urlStr.size() > /* props->maxUrlLength() */ 50)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryLongUrlStorageType);
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
		m_data->addPageInfo(pageInfo, DataCollection::NonAsciiCharacterUrlStorageType);
	}
}

void ModelController::processPageInfoTitle(PageInfoPtr pageInfo) noexcept
{
	int statusCode = pageInfo->itemValue(PageInfo::StatusCodeItemType).toInt();
	const QString title = pageInfo->itemValue(PageInfo::TitleItemType).toString();
	const QString h1 = pageInfo->itemValue(PageInfo::FirstH1ItemType).toString();

	//
	// Replace 301 and 302 constants
	//
	if (title.isEmpty() ||
		statusCode == 301 || statusCode == 302)
	{
		m_data->addPageInfo(pageInfo, DataCollection::EmptyTitleUrlStorageType);
	}
	else if (title.size() > /* props->maxTitleLength() */ 50)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryLongTitleUrlStorageType);
	}
	else if (title.size() < /* props->minTitleLength() */ 5)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryLongTitleUrlStorageType);
	}

	if (!title.isEmpty())
	{
		if (m_data->isPageInfoExists(pageInfo, DataCollection::AllTitlesUrlStorageType))
		{
			PageInfoPtr duplicate = m_data->getPageInfo(pageInfo, DataCollection::AllTitlesUrlStorageType);
			m_data->addPageInfo(pageInfo, DataCollection::DuplicatedTitleUrlStorageType);
			m_data->addPageInfo(duplicate, DataCollection::DuplicatedTitleUrlStorageType);
		}
		else
		{
			m_data->addPageInfo(pageInfo, DataCollection::AllTitlesUrlStorageType);
		}
	}

	if (h1 == title)
	{
		m_data->addPageInfo(pageInfo, DataCollection::DuplicatedH1TitleUrlStorageType);
	}
	
	if (pageInfo->itemValue(PageInfo::HasSeveralTitleTagsItemType).toBool())
	{
		m_data->addPageInfo(pageInfo, DataCollection::SeveralTitleUrlStorageType);
	}
}

void ModelController::processPageInfoMetaDescription(PageInfoPtr pageInfo) noexcept
{
	const int metaDescriptionLength = pageInfo->itemValue(PageInfo::MetaDescriptionLengthItemType).toInt();

	if (metaDescriptionLength == 0)
	{
		m_data->addPageInfo(pageInfo, DataCollection::EmptyMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength > /* props->maxMetaDescriptionLength */ 50)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryLongMetaDescriptionUrlStorageType);
	}
	else if (metaDescriptionLength < /* props->maxMetaDescriptionLength */ 5)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryShortMetaDescriptionUrlStorageType);
	}

	if (metaDescriptionLength > 0)
	{
		if (m_data->isPageInfoExists(pageInfo, DataCollection::AllMetaDescriptionsUrlStorageType))
		{
			PageInfoPtr duplicate = m_data->getPageInfo(pageInfo, DataCollection::AllMetaDescriptionsUrlStorageType);
			m_data->addPageInfo(pageInfo, DataCollection::DuplicatedMetaDescriptionUrlStorageType);
			m_data->addPageInfo(duplicate, DataCollection::DuplicatedMetaDescriptionUrlStorageType);
		}
		else
		{
			m_data->addPageInfo(pageInfo, DataCollection::AllMetaDescriptionsUrlStorageType);
		}
	}

	if (pageInfo->itemValue(PageInfo::HasSeveralMetaDescriptionTagsItemType).toBool())
	{
		m_data->addPageInfo(pageInfo, DataCollection::SeveralMetaDescriptionUrlStorageType);
	}
}

void ModelController::processPageInfoMetaKeywords(PageInfoPtr pageInfo) noexcept
{
	const int metaKeywordsLength = pageInfo->itemValue(PageInfo::MetaKeywordsLengthItemType).toInt();

	if (metaKeywordsLength == 0)
	{
		m_data->addPageInfo(pageInfo, DataCollection::EmptyMetaKeywordsUrlStorageType);
	}

	if (metaKeywordsLength > 0)
	{
		if (m_data->isPageInfoExists(pageInfo, DataCollection::AllMetaKeywordsUrlStorageType))
		{
			PageInfoPtr duplicate = m_data->getPageInfo(pageInfo, DataCollection::AllMetaKeywordsUrlStorageType);
			m_data->addPageInfo(pageInfo, DataCollection::DuplicatedMetaKeywordsUrlStorageType);
			m_data->addPageInfo(duplicate, DataCollection::DuplicatedMetaKeywordsUrlStorageType);
		}
		else
		{
			m_data->addPageInfo(pageInfo, DataCollection::AllMetaKeywordsUrlStorageType);
		}
	}
	

	if (pageInfo->itemValue(PageInfo::HasSeveralMetaKeywordsTagsItemType).toBool())
	{
		m_data->addPageInfo(pageInfo, DataCollection::SeveralMetaKeywordsUrlStorageType);
	}
}

void ModelController::processPageInfoH1(PageInfoPtr pageInfo) noexcept
{
	const int h1Length = pageInfo->itemValue(PageInfo::FirstH1LengthItemType).toInt();

	if (h1Length == 0)
	{
		m_data->addPageInfo(pageInfo, DataCollection::MissingH1UrlStorageType);
	}
	else if (h1Length > /* props->maxH1Length() */ 50)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryLongH1UrlStorageType);
	}

	if (h1Length > 0)
	{
		if (m_data->isPageInfoExists(pageInfo, DataCollection::AllH1UrlStorageType))
		{
			PageInfoPtr duplicate = m_data->getPageInfo(pageInfo, DataCollection::AllH1UrlStorageType);
			m_data->addPageInfo(pageInfo, DataCollection::DuplicatedH1UrlStorageType);
			m_data->addPageInfo(duplicate, DataCollection::DuplicatedH1UrlStorageType);
		}
		else
		{
			m_data->addPageInfo(pageInfo, DataCollection::AllH1UrlStorageType);
		}
	}

	if (pageInfo->itemValue(PageInfo::HasSeveralH1TagsItemType).toBool())
	{
		m_data->addPageInfo(pageInfo, DataCollection::SeveralH1UrlStorageType);
	}

}

void ModelController::processPageInfoH2(PageInfoPtr pageInfo) noexcept
{
	const int h2Length = pageInfo->itemValue(PageInfo::FirstH2LengthItemType).toInt();

	if (h2Length == 0)
	{
		m_data->addPageInfo(pageInfo, DataCollection::MissingH2UrlStorageType);
	}
	else if (h2Length > /* props->maxH2Length() */ 50)
	{
		m_data->addPageInfo(pageInfo, DataCollection::VeryLongH2UrlStorageType);
	}

	if (h2Length > 0)
	{
		if (m_data->isPageInfoExists(pageInfo, DataCollection::AllH2UrlStorageType))
		{
			PageInfoPtr duplicate = m_data->getPageInfo(pageInfo, DataCollection::AllH2UrlStorageType);
			m_data->addPageInfo(pageInfo, DataCollection::DuplicatedH2UrlStorageType);
			m_data->addPageInfo(duplicate, DataCollection::DuplicatedH2UrlStorageType);
		}
		else
		{
			m_data->addPageInfo(pageInfo, DataCollection::AllH2UrlStorageType);
		}
	}

	if (pageInfo->itemValue(PageInfo::HasSeveralH2TagsItemType).toBool())
	{
		m_data->addPageInfo(pageInfo, DataCollection::SeveralH2UrlStorageType);
	}
}

void ModelController::processPageInfoImage(PageInfoPtr pageInfo) noexcept
{
	const int imageCount = pageInfo->itemValue(PageInfo::ImageCountItemType).toInt();

	bool veryLongAltText = false;
	bool missingAltText = false;
	bool over100Kb = false;

	for (int i = 0; i < imageCount; ++i)
	{
		const int altLength = pageInfo->itemValue(PageInfo::ImageAltTextLengthItemType, i).toInt();
		const int sizeKB = pageInfo->itemValue(PageInfo::ImageSizeKbItemType, i).toInt();
		if (!veryLongAltText && altLength > /* props->maxImageAltTextLengt()*/ 50)
		{
			m_data->addPageInfo(pageInfo, DataCollection::VeryLongAltTextImageStorageType);
			veryLongAltText = true;
		}

		if (!missingAltText && altLength == 0)
		{
			m_data->addPageInfo(pageInfo, DataCollection::MissingAltTextImageStorageType);
			missingAltText = true;
		}

		if (!over100Kb && sizeKB > 100)
		{
			m_data->addPageInfo(pageInfo, DataCollection::Over100kbImageStorageType);
			over100Kb = true;
		}

		if (veryLongAltText && missingAltText && over100Kb)
		{
			break;
		}
	}
}

}