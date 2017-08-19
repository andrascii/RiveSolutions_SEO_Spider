#include "storage_adaptor_factory.h"
#include "storage_adaptor.h"
#include "page_info.h"

namespace QuickieWebBot
{

StorageAdaptorFactory::StorageAdaptorFactory(WebCrawler::DataCollection* dataCollection)
	: m_dataCollection(dataCollection)
{
}

StorageAdaptor* StorageAdaptorFactory::create(SummaryCategoryItem type)
{
	assert(type > SummaryCategoryItem::SummaryCategoryItemBegin && 
		type < SummaryCategoryItem::SummaryCategoryItemEnd);

	WebCrawler::DataCollection::StorageType storageType = 
		static_cast<WebCrawler::DataCollection::StorageType>(type);

	if (m_storageAdaptors.find(type) != std::end(m_storageAdaptors))
	{
		return m_storageAdaptors[type];
	}

	StorageAdaptor* storageAdaptor = new StorageAdaptor(m_dataCollection->guiStorage(storageType), storageType, m_dataCollection);

	setupAvailableColumns(storageAdaptor, type);

	m_storageAdaptors[type] = storageAdaptor;

	return storageAdaptor;
}

void StorageAdaptorFactory::setupAvailableColumns(StorageAdaptor* storageAdaptor, SummaryCategoryItem type) const
{
	switch (type)
	{
		case SummaryCategoryItem::SummaryCategoryItemAllPages:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::TitleItemType
				<< PageInfo::TitleLengthItemType
				<< PageInfo::MetaRefreshItemType
				<< PageInfo::MetaRobotsItemType
				<< PageInfo::MetaDescriptionItemType
				<< PageInfo::MetaDescriptionLengthItemType
				<< PageInfo::MetaKeywordsItemType
				<< PageInfo::MetaKeywordsLengthItemType
				<< PageInfo::RedirectedUrlItemType
				<< PageInfo::FirstH1ItemType
				<< PageInfo::FirstH1LengthItemType
				<< PageInfo::SecondH1ItemType
				<< PageInfo::SecondH1LengthItemType
				<< PageInfo::FirstH2ItemType
				<< PageInfo::FirstH2LengthItemType
				<< PageInfo::SecondH2ItemType
				<< PageInfo::SecondH2LengthItemType
				<< PageInfo::CanonicalLinkElementItemType
				<< PageInfo::StatusCodeItemType
				<< PageInfo::PageSizeKbItemType
				<< PageInfo::WordCountItemType
				<< PageInfo::PageHashItemType
				<< PageInfo::UrlLengthItemType
			);

			return;
		}

		//
		// Links available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemUpperCaseLinks:
		case SummaryCategoryItem::SummaryCategoryItemNonAsciiLinks:
		case SummaryCategoryItem::SummaryCategoryItemVeryLongLinks:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::UrlLengthItemType
				<< PageInfo::ContentItemType
				<< PageInfo::StatusCodeItemType
			);

			return;
		}

		//
		// Title available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemEmptyTitles:
		case SummaryCategoryItem::SummaryCategoryItemDuplicatedTitles:
		case SummaryCategoryItem::SummaryCategoryItemVeryLongTitles:
		case SummaryCategoryItem::SummaryCategoryItemVeryShortTitles:
		case SummaryCategoryItem::SummaryCategoryItemDuplicatedH1Titles:
		case SummaryCategoryItem::SummaryCategoryItemSeveralTitlesOnPage:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::TitleItemType
				<< PageInfo::TitleLengthItemType
			);

			return;
		}

		//
		// Meta description available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemEmptyMetaDescriptions:
		case SummaryCategoryItem::SummaryCategoryItemDuplicatedMetaDescriptions:
		case SummaryCategoryItem::SummaryCategoryItemVeryLongMetaDescriptions:
		case SummaryCategoryItem::SummaryCategoryItemVeryShortMetaDescriptions:
		case SummaryCategoryItem::SummaryCategoryItemSeveralMetaDescriptionsOnPage:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::MetaDescriptionItemType
				<< PageInfo::MetaDescriptionLengthItemType
			);

			return;
		}

		//
		// Meta keywords available columns
		// 
		case SummaryCategoryItem::SummaryCategoryItemEmptyMetaKeywords:
		case SummaryCategoryItem::SummaryCategoryItemDuplicatedMetaKeywords:
		case SummaryCategoryItem::SummaryCategoryItemSeveralMetaKeywordsOnPage:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::MetaKeywordsItemType
				<< PageInfo::MetaKeywordsLengthItemType
			);

			return;
		}

		//
		// H1 available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemMissingH1s:
		case SummaryCategoryItem::SummaryCategoryItemDuplicatedH1s:
		case SummaryCategoryItem::SummaryCategoryItemVeryLongH1s:
		case SummaryCategoryItem::SummaryCategoryItemSeveralH1s:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::FirstH1ItemType
				<< PageInfo::FirstH1LengthItemType
				<< PageInfo::SecondH1ItemType
				<< PageInfo::SecondH1LengthItemType
			);

			return;
		}

		//
		// H2 available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemMissingH2s:
		case SummaryCategoryItem::SummaryCategoryItemDuplicatedH2s:
		case SummaryCategoryItem::SummaryCategoryItemVeryLongH2s:
		case SummaryCategoryItem::SummaryCategoryItemSeveralH2s:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::FirstH2ItemType
				<< PageInfo::FirstH2LengthItemType
				<< PageInfo::SecondH2ItemType
				<< PageInfo::SecondH2LengthItemType
			);

			return;
		}

		//
		// Images available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemImagesOver100kb:
		case SummaryCategoryItem::SummaryCategoryItemImageMissingAltText:
		case SummaryCategoryItem::SummaryCategoryItemImagesVeryLongAltText:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ImageAltTextItemType
				<< PageInfo::ImageAltTextLengthItemType
				<< PageInfo::ImageSizeKbItemType
			);

			return;
		}
	}
}

}