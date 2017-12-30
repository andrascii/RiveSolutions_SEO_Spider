#include "storage_adapter_factory.h"
#include "parsed_page_info.h"
#include "parsed_page_info_storage_adapter.h"

namespace SeoSpider
{

IStorageAdapter* StorageAdapterFactory::createParsedPageInfoStorage(StorageAdapterType type, 
	const CrawlerEngine::SequencedDataCollection* sequencedDataCollection) const
{
	ASSERT(type > StorageAdapterType::StorageAdapterTypeBegin &&
		type < StorageAdapterType::StorageAdapterTypeEnd);

	const CrawlerEngine::StorageType storageType =
		static_cast<CrawlerEngine::StorageType>(type);

	IParsedPageStorageAdapter* storageAdapter = new ParsedPageInfoStorageAdapter(
		sequencedDataCollection->storage(storageType), storageType);

	setupAvailableColumns(storageAdapter, type);

	return storageAdapter;
}


IStorageAdapter* StorageAdapterFactory::createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer)
{
	IPageLinksStorageAdapter* storageAdapter = new PageLinksStorageAdapter(associatedPageRawInfoPointer, type);;

	setupAvailablePageLinkColumns(storageAdapter);

	return storageAdapter;
}

void StorageAdapterFactory::setupAvailableColumns(IParsedPageStorageAdapter* storageAdapter, StorageAdapterType type)
{
	switch (type)
	{
		case StorageAdapterType::StorageAdapterTypeAllPages:
		case StorageAdapterType::StorageAdapterTypeHtmlResources:
		case StorageAdapterType::StorageAdapterTypeExternalHtmlResources:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
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
		case StorageAdapterType::StorageAdapterTypeImageResources:
		case StorageAdapterType::StorageAdapterTypeExternalImageResources:
		case StorageAdapterType::StorageAdapterTypeJavaScriptResources:
		case StorageAdapterType::StorageAdapterTypeExternalJavaScriptResources:
		case StorageAdapterType::StorageAdapterTypeStyleSheetResources:
		case StorageAdapterType::StorageAdapterTypeExternalStyleSheetResources:
		case StorageAdapterType::StorageAdapterTypeFlashResources:
		case StorageAdapterType::StorageAdapterTypeExternalFlashResources:
		case StorageAdapterType::StorageAdapterTypeOtherResources:
		case StorageAdapterType::StorageAdapterTypeExternalOtherResources:
		case StorageAdapterType::StorageAdapterTypeVideoResources:
		case StorageAdapterType::StorageAdapterTypeExternalVideoResources:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
			);

			return;
		}

		//
		// Links available columns
		//
		case StorageAdapterType::StorageAdapterTypeUpperCaseLinks:
		case StorageAdapterType::StorageAdapterTypeNonAsciiLinks:
		case StorageAdapterType::StorageAdapterTypeTooLongLinks:
		case StorageAdapterType::StorageAdapterTypeBrokenLinks:
		case StorageAdapterType::StorageAdapterTypeStatus4xx:
		case StorageAdapterType::StorageAdapterTypeStatus5xx:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
			);

			return;
		}
		case StorageAdapterType::StorageAdapterTypeStatus301:
		case StorageAdapterType::StorageAdapterTypeStatus302:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::RedirectedUrlColumn
			);

			return;
		}

		//
		// Title available columns
		//
		case StorageAdapterType::StorageAdapterTypeEmptyTitles:
		case StorageAdapterType::StorageAdapterTypeDuplicatedTitles:
		case StorageAdapterType::StorageAdapterTypeTooLongTitles:
		case StorageAdapterType::StorageAdapterTypeTooShortTitles:
		case StorageAdapterType::StorageAdapterTypeSeveralTitlesOnPage:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::TitleLengthColumn
			);

			return;
		}
		case StorageAdapterType::StorageAdapterTypeDuplicatedH1Titles:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::TitleLengthColumn
				<< ParsedPageInfo::Column::FirstH1Column
				<< ParsedPageInfo::Column::FirstH1LengthColumn
			);

			return;
		}

		//
		// Meta description available columns
		//
		case StorageAdapterType::StorageAdapterTypeEmptyMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeDuplicatedMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeTooLongMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeSeveralMetaDescriptionsOnPage:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
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
		case StorageAdapterType::StorageAdapterTypeEmptyMetaKeywords:
		case StorageAdapterType::StorageAdapterTypeDuplicatedMetaKeywords:
		case StorageAdapterType::StorageAdapterTypeSeveralMetaKeywordsOnPage:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
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
		case StorageAdapterType::StorageAdapterTypeMissingH1s:
		case StorageAdapterType::StorageAdapterTypeDuplicatedH1s:
		case StorageAdapterType::StorageAdapterTypeTooLongH1s:
		case StorageAdapterType::StorageAdapterTypeSeveralH1s:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
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
		case StorageAdapterType::StorageAdapterTypeMissingH2s:
		case StorageAdapterType::StorageAdapterTypeDuplicatedH2s:
		case StorageAdapterType::StorageAdapterTypeTooLongH2s:
		case StorageAdapterType::StorageAdapterTypeSeveralH2s:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
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
		case StorageAdapterType::StorageAdapterTypeImagesOver100kb:
		case StorageAdapterType::StorageAdapterTypeImageMissingAltText:
		case StorageAdapterType::StorageAdapterTypeImagesTooLongAltText:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::ImageSizeKbColumn
			);

			return;
		}

		case StorageAdapterType::StorageAdapterTypeImagesTooManyLinksOnPage:
		{
			storageAdapter->setAvailableColumns(QList<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::LinksOnThisPageCountColumn
			);

			return;
		}
	}
}


void StorageAdapterFactory::setupAvailablePageLinkColumns(IPageLinksStorageAdapter* storageAdapter)
{
	storageAdapter->setAvailableColumns(QList<ParsedPageInfo::PageLinksColumn>()
		<< ParsedPageInfo::PageLinksColumn::UrlColumn
		<< ParsedPageInfo::PageLinksColumn::StatusCodeColumn
		<< ParsedPageInfo::PageLinksColumn::AltOrTitleColumn
		<< ParsedPageInfo::PageLinksColumn::LinkParameterColumn
	);
}

}