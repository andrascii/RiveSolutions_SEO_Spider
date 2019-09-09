#include "stdafx.h"
#include "storage_adapter_factory.h"
#include "parsed_page_info.h"
#include "parsed_page_info_storage_adapter.h"

namespace SeoSpider
{

IStorageAdapter* StorageAdapterFactory::createParsedPageInfoStorage(StorageAdapterType type,
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection) const
{
	ASSERT(type > StorageAdapterType::StorageAdapterTypeBegin &&
		type < StorageAdapterType::StorageAdapterTypeEnd);

	const CrawlerEngine::StorageType storageType = type == StorageAdapterType::StorageAdapterTypeCustomDataFeed
		? CrawlerEngine::CrawledUrlStorageType
		: static_cast<CrawlerEngine::StorageType>(type);

	IParsedPageStorageAdapter* storageAdapter = new ParsedPageInfoStorageAdapter(sequencedDataCollection,
		sequencedDataCollection->storage(storageType), storageType);

	storageAdapter->setCurrentColumns(parsedPageAvailableColumns(type));
	storageAdapter->setAvailableColumns(parsedPageAllColumns());

	return storageAdapter;
}


IStorageAdapter* StorageAdapterFactory::createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer)
{
	IPageLinksStorageAdapter* storageAdapter = new PageLinksStorageAdapter(associatedPageRawInfoPointer, type);;

	setupAvailablePageLinkColumns(storageAdapter);

	return storageAdapter;
}

QVector<ParsedPageInfo::Column> StorageAdapterFactory::parsedPageAllColumns()
{
	return QVector<ParsedPageInfo::Column>()
		<< ParsedPageInfo::Column::UrlColumn
		<< ParsedPageInfo::Column::UrlLengthColumn
		<< ParsedPageInfo::Column::LinksOnThisPageCountColumn
		<< ParsedPageInfo::Column::LinksToThisPageCountColumn
		<< ParsedPageInfo::Column::ContentTypeColumn
		<< ParsedPageInfo::Column::StatusCodeColumn
		<< ParsedPageInfo::Column::SchemeColumn
		<< ParsedPageInfo::Column::ResponseTimeColumn
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
		<< ParsedPageInfo::Column::PageSizeKbColumn
		<< ParsedPageInfo::Column::WordCountColumn
		<< ParsedPageInfo::Column::PageHashColumn;
}

QVector<ParsedPageInfo::Column> StorageAdapterFactory::parsedPageAvailableColumns(StorageAdapterType type)
{
	switch (type)
	{
		case StorageAdapterType::StorageAdapterTypeAllPages:
		case StorageAdapterType::StorageAdapterTypeHtmlResources:
		case StorageAdapterType::StorageAdapterTypeExternalHtmlResources:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::LinksOnThisPageCountColumn
				<< ParsedPageInfo::Column::LinksToThisPageCountColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn
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
				<< ParsedPageInfo::Column::PageSizeKbColumn
				<< ParsedPageInfo::Column::WordCountColumn
				<< ParsedPageInfo::Column::PageHashColumn;
		}

		// Resources columns
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
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn;

		}

		// Links available columns
		case StorageAdapterType::StorageAdapterTypeUpperCaseLinks:
		case StorageAdapterType::StorageAdapterTypeNonAsciiLinks:
		case StorageAdapterType::StorageAdapterTypeTooLongLinks:
		case StorageAdapterType::StorageAdapterTypeBrokenLinks:
		case StorageAdapterType::StorageAdapterTypeStatus4xx:
		case StorageAdapterType::StorageAdapterTypeStatus5xx:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn;
		}
		case StorageAdapterType::StorageAdapterTypeStatus301:
		case StorageAdapterType::StorageAdapterTypeStatus302:
		case StorageAdapterType::StorageAdapterTypeTooManyRedirections:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn
				<< ParsedPageInfo::Column::RedirectedUrlColumn;
		}
		case StorageAdapterType::StorageAdapterTypeTimeout:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn;
		}

		// Title available columns
		case StorageAdapterType::StorageAdapterTypeEmptyTitles:
		case StorageAdapterType::StorageAdapterTypeDuplicatedTitles:
		case StorageAdapterType::StorageAdapterTypeTooLongTitles:
		case StorageAdapterType::StorageAdapterTypeTooShortTitles:
		case StorageAdapterType::StorageAdapterTypeSeveralTitlesOnPage:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::TitleLengthColumn;
		}
		case StorageAdapterType::StorageAdapterTypeDuplicatedH1Titles:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::TitleLengthColumn
				<< ParsedPageInfo::Column::FirstH1Column
				<< ParsedPageInfo::Column::FirstH1LengthColumn;
		}

		// Meta description available columns
		case StorageAdapterType::StorageAdapterTypeEmptyMetaDescriptions:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn;
		}
		case StorageAdapterType::StorageAdapterTypeDuplicatedMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeTooLongMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions:
		case StorageAdapterType::StorageAdapterTypeSeveralMetaDescriptionsOnPage:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::MetaDescriptionColumn
				<< ParsedPageInfo::Column::MetaDescriptionLengthColumn;
		}

		// Meta keywords available columns
		case StorageAdapterType::StorageAdapterTypeEmptyMetaKeywords:
		case StorageAdapterType::StorageAdapterTypeDuplicatedMetaKeywords:
		case StorageAdapterType::StorageAdapterTypeSeveralMetaKeywordsOnPage:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::MetaKeywordsColumn
				<< ParsedPageInfo::Column::MetaKeywordsLengthColumn;
		}

		// H1 available columns
		case StorageAdapterType::StorageAdapterTypeMissingH1s:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn;
		}
		case StorageAdapterType::StorageAdapterTypeDuplicatedH1s:
		case StorageAdapterType::StorageAdapterTypeTooLongH1s:
		case StorageAdapterType::StorageAdapterTypeSeveralH1s:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::FirstH1Column
				<< ParsedPageInfo::Column::FirstH1LengthColumn
				<< ParsedPageInfo::Column::SecondH1Column
				<< ParsedPageInfo::Column::SecondH1LengthColumn;
		}

		// H2 available columns
		case StorageAdapterType::StorageAdapterTypeMissingH2s:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn;
		}
		case StorageAdapterType::StorageAdapterTypeDuplicatedH2s:
		case StorageAdapterType::StorageAdapterTypeTooLongH2s:
		case StorageAdapterType::StorageAdapterTypeSeveralH2s:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ContentTypeColumn
				<< ParsedPageInfo::Column::FirstH2Column
				<< ParsedPageInfo::Column::FirstH2LengthColumn
				<< ParsedPageInfo::Column::SecondH2Column
				<< ParsedPageInfo::Column::SecondH2LengthColumn;
		}

		// Images available columns
		case StorageAdapterType::StorageAdapterTypeImagesOver100kb:
		case StorageAdapterType::StorageAdapterTypeImageMissingAltText:
		case StorageAdapterType::StorageAdapterTypeImagesTooLongAltText:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::ImageSizeKbColumn;
		}

		// page problems
		case StorageAdapterType::StorageAdapterTypeTooManyLinksOnPage:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::LinksOnThisPageCountColumn;
		}
		case StorageAdapterType::StorageAdapterTypeContainsMetaRefreshTag:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn
				<< ParsedPageInfo::Column::MetaRefreshColumn;
		}
		case StorageAdapterType::StorageAdapterTypeContainsFrames:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn;
		}

		// not indexed pages
		case StorageAdapterType::StorageAdapterTypeBlockedForSEIndexing:
		case StorageAdapterType::StorageAdapterTypeNofollowLinks:
		case StorageAdapterType::StorageAdapterTypeBlockedByRobotsTxt:
		case StorageAdapterType::StorageAdapterTypeBlockedByXRobotsTag:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::UrlLengthColumn;
		}

		// custom data feed
		case StorageAdapterType::StorageAdapterTypeCustomDataFeed:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn;
		}

		// yandex metrica
		case StorageAdapterType::StorageAdapterTypeYandexMetricaCounter1:
		case StorageAdapterType::StorageAdapterTypeYandexMetricaCounter2:
		case StorageAdapterType::StorageAdapterTypeYandexMetricaCounter3:
		case StorageAdapterType::StorageAdapterTypeYandexMetricaCounter4:
		case StorageAdapterType::StorageAdapterTypeYandexMetricaCounter5:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn;
		}

		// Google Analytics
		case StorageAdapterType::StorageAdapterTypeGoogleAnalyticsCounter1:
		case StorageAdapterType::StorageAdapterTypeGoogleAnalyticsCounter2:
		case StorageAdapterType::StorageAdapterTypeGoogleAnalyticsCounter3:
		case StorageAdapterType::StorageAdapterTypeGoogleAnalyticsCounter4:
		case StorageAdapterType::StorageAdapterTypeGoogleAnalyticsCounter5:
		{
			return QVector<ParsedPageInfo::Column>()
				<< ParsedPageInfo::Column::UrlColumn
				<< ParsedPageInfo::Column::TitleColumn
				<< ParsedPageInfo::Column::StatusCodeColumn
				<< ParsedPageInfo::Column::SchemeColumn
				<< ParsedPageInfo::Column::ResponseTimeColumn;
		}
	}

	ASSERT(!"Invalid type");
	return QVector<ParsedPageInfo::Column>();
}

void StorageAdapterFactory::setupAvailablePageLinkColumns(IPageLinksStorageAdapter* storageAdapter)
{
	storageAdapter->setAvailableColumns(QVector<ParsedPageInfo::PageLinksColumn>()
		<< ParsedPageInfo::PageLinksColumn::UrlColumn
		<< ParsedPageInfo::PageLinksColumn::StatusCodeColumn
		<< ParsedPageInfo::PageLinksColumn::AltOrTitleColumn
		<< ParsedPageInfo::PageLinksColumn::LinkParameterColumn
	);
}

}