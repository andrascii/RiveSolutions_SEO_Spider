#include "storage_adaptor_factory.h"
#include "page_raw_info.h"
#include "page_raw_info_storage_adaptor.h"

namespace QuickieWebBot
{

IStorageAdaptor* StorageAdaptorFactory::createPageRawInfoStorage(StorageAdaptorType type, WebCrawler::GuiStorage* guiStorage)
{
	ASSERT(type > StorageAdaptorType::StorageAdaptorTypeBegin &&
		type < StorageAdaptorType::StorageAdaptorTypeEnd);

	WebCrawler::DataCollection::StorageType storageType = 
		static_cast<WebCrawler::DataCollection::StorageType>(type);

	IStorageAdaptor* storageAdaptor = new PageRawInfoStorageAdaptor(guiStorage->guiStorage(storageType), storageType, guiStorage);

	setupAvailableColumns(storageAdaptor, type);

	return storageAdaptor;
}


IStorageAdaptor* StorageAdaptorFactory::createPageLinksStorage(PageLinkType type, PageRawInfoPtr associatedPageRawInfoPointer)
{
	IStorageAdaptor* storageAdaptor = nullptr;

	storageAdaptor = new PageLinksStorageAdaptor(associatedPageRawInfoPointer, type);

	setupAvailablePageLinkColumns(storageAdaptor);

	return storageAdaptor;
}

void StorageAdaptorFactory::setupAvailableColumns(IStorageAdaptor* storageAdaptor, StorageAdaptorType type) const
{
	switch (type)
	{
		case StorageAdaptorType::StorageAdaptorTypeAllPages:
		case StorageAdaptorType::StorageAdaptorTypeHtmlResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalHtmlResources:
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
		case StorageAdaptorType::StorageAdaptorTypeImageResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalImageResources:
		case StorageAdaptorType::StorageAdaptorTypeJavaScriptResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalJavaScriptResources:
		case StorageAdaptorType::StorageAdaptorTypeStyleSheetResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalStyleSheetResources:
		case StorageAdaptorType::StorageAdaptorTypeFlashResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalFlashResources:
		case StorageAdaptorType::StorageAdaptorTypeOtherResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalOtherResources:
		case StorageAdaptorType::StorageAdaptorTypeVideoResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalVideoResources:
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
		case StorageAdaptorType::StorageAdaptorTypeUpperCaseLinks:
		case StorageAdaptorType::StorageAdaptorTypeNonAsciiLinks:
		case StorageAdaptorType::StorageAdaptorTypeVeryLongLinks:
		case StorageAdaptorType::StorageAdaptorTypeBrokenLinks:
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
		case StorageAdaptorType::StorageAdaptorTypeEmptyTitles:
		case StorageAdaptorType::StorageAdaptorTypeDuplicatedTitles:
		case StorageAdaptorType::StorageAdaptorTypeVeryLongTitles:
		case StorageAdaptorType::StorageAdaptorTypeVeryShortTitles:
		case StorageAdaptorType::StorageAdaptorTypeDuplicatedH1Titles:
		case StorageAdaptorType::StorageAdaptorTypeSeveralTitlesOnPage:
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
		case StorageAdaptorType::StorageAdaptorTypeEmptyMetaDescriptions:
		case StorageAdaptorType::StorageAdaptorTypeDuplicatedMetaDescriptions:
		case StorageAdaptorType::StorageAdaptorTypeVeryLongMetaDescriptions:
		case StorageAdaptorType::StorageAdaptorTypeVeryShortMetaDescriptions:
		case StorageAdaptorType::StorageAdaptorTypeSeveralMetaDescriptionsOnPage:
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
		case StorageAdaptorType::StorageAdaptorTypeEmptyMetaKeywords:
		case StorageAdaptorType::StorageAdaptorTypeDuplicatedMetaKeywords:
		case StorageAdaptorType::StorageAdaptorTypeSeveralMetaKeywordsOnPage:
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
		case StorageAdaptorType::StorageAdaptorTypeMissingH1s:
		case StorageAdaptorType::StorageAdaptorTypeDuplicatedH1s:
		case StorageAdaptorType::StorageAdaptorTypeVeryLongH1s:
		case StorageAdaptorType::StorageAdaptorTypeSeveralH1s:
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
		case StorageAdaptorType::StorageAdaptorTypeMissingH2s:
		case StorageAdaptorType::StorageAdaptorTypeDuplicatedH2s:
		case StorageAdaptorType::StorageAdaptorTypeVeryLongH2s:
		case StorageAdaptorType::StorageAdaptorTypeSeveralH2s:
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
		case StorageAdaptorType::StorageAdaptorTypeImagesOver100kb:
		case StorageAdaptorType::StorageAdaptorTypeImageMissingAltText:
		case StorageAdaptorType::StorageAdaptorTypeImagesVeryLongAltText:
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


void StorageAdaptorFactory::setupAvailablePageLinkColumns(IStorageAdaptor* storageAdaptor)
{
	storageAdaptor->setAvailableColumns(QList<PageRawInfo::Column>()
		<< PageRawInfo::UrlItemType
		<< PageRawInfo::StatusCodeItemType
		<< PageRawInfo::AltTextItemType
		<< PageRawInfo::NoFollowDoFollowLinkItemType
	);
}

}