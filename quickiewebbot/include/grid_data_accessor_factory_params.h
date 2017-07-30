#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

struct GridDataAccessorFactoryParams
{
	enum Type
	{
		TypeInvalid = -1,

		TypeAllCrawledUrls = DataCollection::CrawledUrlStorageType,
		TypeAllExternalUrsl = DataCollection::ExternalUrlStorageType,
		TypeAllUppercaseLinks = DataCollection::UpperCaseUrlStorageType,
		TypeAllNonAsciiLinks = DataCollection::NonAsciiCharacterUrlStorageType,
		TypeAllVeryLongLinks = DataCollection::VeryLongUrlStorageType,
		TypeAllEmptyTitle = DataCollection::EmptyTitleUrlStorageType,
		TypeAllDuplicatedTitle = DataCollection::DuplicatedTitleUrlStorageType,
		TypeAllVeryLongTitle = DataCollection::VeryLongTitleUrlStorageType,
		TypeAllVeryShortTitle = DataCollection::VeryShortTitleUrlStorageType,
		TypeAllDuplicatedH1Title = DataCollection::DuplicatedH1TitleUrlStorageType,
		TypeAllSeveralTitle = DataCollection::SeveralTitleUrlStorageType,
		TypeAllEmptyMetaDescription = DataCollection::EmptyMetaDescriptionUrlStorageType,
		TypeAllDuplicatedMetaDescription = DataCollection::DuplicatedMetaDescriptionUrlStorageType,
		TypeAllVeryLongMetaDescription = DataCollection::VeryLongMetaDescriptionUrlStorageType,
		TypeAllVeryShortMetaDescription = DataCollection::VeryShortMetaDescriptionUrlStorageType,
		TypeAllSeveralMetaDescription = DataCollection::SeveralMetaDescriptionUrlStorageType,
		TypeAllEmptyMetaKeywords = DataCollection::EmptyMetaKeywordsUrlStorageType,
		TypeAllDuplicatedMetaKeywords = DataCollection::DuplicatedMetaKeywordsUrlStorageType,
		TypeAllSeveralMetaKeywords = DataCollection::SeveralMetaKeywordsUrlStorageType,
		TypeAllMissingH1 = DataCollection::MissingH1UrlStorageType,
		TypeAllDuplicatedH1 = DataCollection::DuplicatedH1UrlStorageType,
		TypeAllVeryLongH1 = DataCollection::VeryLongH1UrlStorageType,
		TypeAllSeveralH1 = DataCollection::SeveralH1UrlStorageType,
		TypeAllMissingH2 = DataCollection::MissingH2UrlStorageType,
		TypeAllDuplicatedH2 = DataCollection::DuplicatedH2UrlStorageType,
		TypeAllVeryLongH2 = DataCollection::VeryLongH2UrlStorageType,
		TypeAllSeveralH2 = DataCollection::SeveralH2UrlStorageType,
		TypeAllOver100kbImages = DataCollection::Over100kbImageStorageType,
		TypeAllMissingAltImages = DataCollection::MissingAltTextImageStorageType,
		TypeAllVeryLongAltTextImages = DataCollection::VeryLongAltTextImageStorageType,
		TypeSummary,
	};

	enum Mode
	{
		ModeGeneral,
		ModeSummary,
		ModeOneRow,
	};

	Type accessorType;
	Mode mode;
	int row;

	GridDataAccessorFactoryParams(Type accessorType, Mode mode = ModeGeneral, int row = -1)
		: accessorType(accessorType)
		, mode(mode)
		, row(row)
	{
	}

	bool isValid() const
	{
		return accessorType != TypeInvalid;
	}

	// TODO: add params

};

}