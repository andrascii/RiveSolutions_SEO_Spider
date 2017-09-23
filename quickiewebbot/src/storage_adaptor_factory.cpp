#include "storage_adaptor_factory.h"
#include "parsed_page_info.h"
#include "parsed_page_info_storage_adaptor.h"

namespace QuickieWebBot
{

IStorageAdaptor* StorageAdaptorFactory::createPageRawInfoStorage(StorageAdaptorType type, WebCrawler::SequencedDataCollection* guiStorage)
{
	ASSERT(type > StorageAdaptorType::StorageAdaptorTypeBegin &&
		type < StorageAdaptorType::StorageAdaptorTypeEnd);

	WebCrawler::StorageType storageType = 
		static_cast<WebCrawler::StorageType>(type);

	IStorageAdaptor* storageAdaptor = new ParsedPageInfoStorageAdaptor(guiStorage->storage(storageType), storageType, guiStorage);

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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::TitleItemType
				<< ParsedPageInfo::TitleLengthItemType
				<< ParsedPageInfo::MetaRefreshItemType
				<< ParsedPageInfo::MetaRobotsItemType
				<< ParsedPageInfo::MetaDescriptionItemType
				<< ParsedPageInfo::MetaDescriptionLengthItemType
				<< ParsedPageInfo::MetaKeywordsItemType
				<< ParsedPageInfo::MetaKeywordsLengthItemType
				<< ParsedPageInfo::RedirectedUrlItemType
				<< ParsedPageInfo::FirstH1ItemType
				<< ParsedPageInfo::FirstH1LengthItemType
				<< ParsedPageInfo::SecondH1ItemType
				<< ParsedPageInfo::SecondH1LengthItemType
				<< ParsedPageInfo::FirstH2ItemType
				<< ParsedPageInfo::FirstH2LengthItemType
				<< ParsedPageInfo::SecondH2ItemType
				<< ParsedPageInfo::SecondH2LengthItemType
				<< ParsedPageInfo::CanonicalLinkElementItemType
				<< ParsedPageInfo::StatusCodeItemType
				<< ParsedPageInfo::PageSizeKbItemType
				<< ParsedPageInfo::WordCountItemType
				<< ParsedPageInfo::PageHashItemType
				<< ParsedPageInfo::UrlLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::StatusCodeItemType
				<< ParsedPageInfo::UrlLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::UrlLengthItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::StatusCodeItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::TitleItemType
				<< ParsedPageInfo::TitleLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::MetaDescriptionItemType
				<< ParsedPageInfo::MetaDescriptionLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::MetaKeywordsItemType
				<< ParsedPageInfo::MetaKeywordsLengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::FirstH1ItemType
				<< ParsedPageInfo::FirstH1LengthItemType
				<< ParsedPageInfo::SecondH1ItemType
				<< ParsedPageInfo::SecondH1LengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::ContentTypeItemType
				<< ParsedPageInfo::FirstH2ItemType
				<< ParsedPageInfo::FirstH2LengthItemType
				<< ParsedPageInfo::SecondH2ItemType
				<< ParsedPageInfo::SecondH2LengthItemType
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
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::UrlItemType
				<< ParsedPageInfo::AltTextItemType
				<< ParsedPageInfo::AltTextLengthItemType
				<< ParsedPageInfo::ImageSizeKbItemType
			);

			return;
		}
	}
}


void StorageAdaptorFactory::setupAvailablePageLinkColumns(IStorageAdaptor* storageAdaptor)
{
	storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
		<< ParsedPageInfo::UrlItemType
		<< ParsedPageInfo::StatusCodeItemType
		<< ParsedPageInfo::AltTextItemType
		<< ParsedPageInfo::NoFollowDoFollowLinkItemType
	);
}

}