#include "storage_adaptor_factory.h"
#include "page_raw_info_storage_adaptor.h"
#include "page_raw_info.h"

namespace QuickieWebBot
{

StorageAdaptorFactory::StorageAdaptorFactory(WebCrawler::GuiStorage* guiStorage)
	: m_guiStorage(guiStorage)
{
}

IStorageAdaptor* StorageAdaptorFactory::create(SummaryCategoryItem type)
{
	ASSERT(type > SummaryCategoryItem::SummaryCategoryItemBegin &&
		type < SummaryCategoryItem::SummaryCategoryItemEnd);

	WebCrawler::DataCollection::StorageType storageType = 
		static_cast<WebCrawler::DataCollection::StorageType>(type);

	if (m_storageAdaptors.find(type) != std::end(m_storageAdaptors))
	{
		return m_storageAdaptors[type];
	}

	IStorageAdaptor* storageAdaptor = new PageRawInfoStorageAdaptor(theApp->guiStorage()->guiStorage(storageType), storageType, m_guiStorage);

	setupAvailableColumns(storageAdaptor, type);

	m_storageAdaptors[type] = storageAdaptor;

	return storageAdaptor;
}

void StorageAdaptorFactory::setupAvailableColumns(IStorageAdaptor* storageAdaptor, SummaryCategoryItem type) const
{
	switch (type)
	{
		case SummaryCategoryItem::SummaryCategoryItemAllPages:
		case SummaryCategoryItem::SummaryCategoryItemHtmlResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalHtmlResources:
		{
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::TitleItemType
				<< PageRawInfo::TitleLengthItemType
				<< PageRawInfo::MetaRefreshItemType
				<< PageRawInfo::MetaRobotsItemType
				<< PageRawInfo::MetaDescriptionItemType
				<< PageRawInfo::MetaDescriptionLengthItemType
				<< PageRawInfo::MetaKeywordsItemType
				<< PageRawInfo::MetaKeywordsLengthItemType
				<< PageRawInfo::RedirectedUrlItemType
				<< PageRawInfo::FirstH1ItemType
				<< PageRawInfo::FirstH1LengthItemType
				<< PageRawInfo::SecondH1ItemType
				<< PageRawInfo::SecondH1LengthItemType
				<< PageRawInfo::FirstH2ItemType
				<< PageRawInfo::FirstH2LengthItemType
				<< PageRawInfo::SecondH2ItemType
				<< PageRawInfo::SecondH2LengthItemType
				<< PageRawInfo::CanonicalLinkElementItemType
				<< PageRawInfo::StatusCodeItemType
				<< PageRawInfo::PageSizeKbItemType
				<< PageRawInfo::WordCountItemType
				<< PageRawInfo::PageHashItemType
				<< PageRawInfo::UrlLengthItemType
			);

			return;
		}
		
		//
		// Resources columns
		//
		case SummaryCategoryItem::SummaryCategoryItemImageResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalImageResources:
		case SummaryCategoryItem::SummaryCategoryItemJavaScriptResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalJavaScriptResources:
		case SummaryCategoryItem::SummaryCategoryItemStyleSheetResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalStyleSheetResources:
		case SummaryCategoryItem::SummaryCategoryItemFlashResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalFlashResources:
		case SummaryCategoryItem::SummaryCategoryItemOtherResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalOtherResources:
		case SummaryCategoryItem::SummaryCategoryItemVideoResources:
		case SummaryCategoryItem::SummaryCategoryItemExternalVideoResources:
		{
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::StatusCodeItemType
				<< PageRawInfo::UrlLengthItemType
			);

			return;
		}

		//
		// Links available columns
		//
		case SummaryCategoryItem::SummaryCategoryItemUpperCaseLinks:
		case SummaryCategoryItem::SummaryCategoryItemNonAsciiLinks:
		case SummaryCategoryItem::SummaryCategoryItemVeryLongLinks:
		case SummaryCategoryItem::SummaryCategoryItemBrokenLinks:
		{
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::UrlLengthItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::StatusCodeItemType
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
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::TitleItemType
				<< PageRawInfo::TitleLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::MetaDescriptionItemType
				<< PageRawInfo::MetaDescriptionLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::MetaKeywordsItemType
				<< PageRawInfo::MetaKeywordsLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::FirstH1ItemType
				<< PageRawInfo::FirstH1LengthItemType
				<< PageRawInfo::SecondH1ItemType
				<< PageRawInfo::SecondH1LengthItemType
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
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::ContentTypeItemType
				<< PageRawInfo::FirstH2ItemType
				<< PageRawInfo::FirstH2LengthItemType
				<< PageRawInfo::SecondH2ItemType
				<< PageRawInfo::SecondH2LengthItemType
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
			storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
				<< PageRawInfo::UrlItemType
				<< PageRawInfo::AltTextItemType
				<< PageRawInfo::AltTextLengthItemType
				<< PageRawInfo::ImageSizeKbItemType
			);

			return;
		}
	}
}

}