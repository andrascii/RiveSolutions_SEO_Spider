#include "storage_adaptor_factory.h"
#include "parsed_page_info.h"
#include "parsed_page_info_storage_adaptor.h"

namespace QuickieWebBot
{

IStorageAdaptor* StorageAdaptorFactory::createPageRawInfoStorage(StorageAdaptorType type, WebCrawler::SequencedDataCollection* guiStorage) const
{
	ASSERT(type > StorageAdaptorType::StorageAdaptorTypeBegin &&
		type < StorageAdaptorType::StorageAdaptorTypeEnd);

	const WebCrawler::StorageType storageType =
		static_cast<WebCrawler::StorageType>(type);

	IParsedPageStorageAdaptor* storageAdaptor = new ParsedPageInfoStorageAdaptor(guiStorage->storage(storageType), storageType, guiStorage);

	setupAvailableColumns(storageAdaptor, type);

	return storageAdaptor;
}


IStorageAdaptor* StorageAdaptorFactory::createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer)
{
	IPageLinksStorageAdaptor* storageAdaptor = new PageLinksStorageAdaptor(associatedPageRawInfoPointer, type);;

	setupAvailablePageLinkColumns(storageAdaptor);

	return storageAdaptor;
}

void StorageAdaptorFactory::setupAvailableColumns(IParsedPageStorageAdaptor* storageAdaptor, StorageAdaptorType type)
{
	switch (type)
	{
		case StorageAdaptorType::StorageAdaptorTypeAllPages:
		case StorageAdaptorType::StorageAdaptorTypeHtmlResources:
		case StorageAdaptorType::StorageAdaptorTypeExternalHtmlResources:
		{
			storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::TitleLengthColumn
				<< ParsedPageInfo::Column::MetaRefreshColumn
				<< ParsedPageInfo::Column::MetaRobotsColumn
				<< ParsedPageInfo::Column::MetaDescriptionColumn
				<< ParsedPageInfo::Column::MetaDescriptionLengthColumn
				<< ParsedPageInfo::Column::MetaKeywordsColumn
				<< ParsedPageInfo::Column::MetaKeywordsLengthColumn
				<< ParsedPageInfo::Column::RedirectedUrlColumn
				<< ParsedPageInfo::Column::FirstH1Column
				<< ParsedPageInfo::Column::FirstH1LengthColumn
				<< ParsedPageInfo::Column::SecondH1Column
				<< ParsedPageInfo::Column::SecondH1LengthColumn
				<< ParsedPageInfo::Column::FirstH2Column
				<< ParsedPageInfo::Column::FirstH2LengthColumn
				<< ParsedPageInfo::Column::SecondH2Column
				<< ParsedPageInfo::Column::SecondH2LengthColumn
				<< ParsedPageInfo::Column::CanonicalLinkElementColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::PageSizeKbColumn
				<< ParsedPageInfo::Column::WordCountColumn
				<< ParsedPageInfo::Column::PageHashColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::TitleLengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::MetaDescriptionColumn
				<< ParsedPageInfo::Column::MetaDescriptionLengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::MetaKeywordsColumn
				<< ParsedPageInfo::Column::MetaKeywordsLengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::FirstH1Column
				<< ParsedPageInfo::Column::FirstH1LengthColumn
				<< ParsedPageInfo::Column::SecondH1Column
				<< ParsedPageInfo::Column::SecondH1LengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::FirstH2Column
				<< ParsedPageInfo::Column::FirstH2LengthColumn
				<< ParsedPageInfo::Column::SecondH2Column
				<< ParsedPageInfo::Column::SecondH2LengthColumn
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
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ImageSizeKbColumn
			);

			return;
		}
	}
}


void StorageAdaptorFactory::setupAvailablePageLinkColumns(IPageLinksStorageAdaptor* storageAdaptor)
{
	storageAdaptor->setAvailableColumns(QList<ParsedPageInfo::PageLinksColumn>()
		<< ParsedPageInfo::PageLinksColumn::UrlColumn
		<< ParsedPageInfo::PageLinksColumn::StatusCodeColumn
		<< ParsedPageInfo::PageLinksColumn::AltOrTitleColumn
		<< ParsedPageInfo::PageLinksColumn::LinkParameterColumn
	);
}

}