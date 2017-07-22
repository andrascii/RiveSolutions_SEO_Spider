#pragma once

namespace QuickieWebBot
{

class IModelDataAccessor;

struct ModelDataAccessorFactoryParams
{
	enum Type 
	{
		TypeInvalid = -1,
		TypeAllCrawledUrls,
		TypeSummary,
		TypeSummaryLinksNonAscii,
		TypeSummaryLinksUpperCase,
		TypeSummaryLinksTooLong,
		TypeSummaryTitlesEmpty,
		TypeSummaryTitlesDuplicates,
		TypeSummaryTitlesTooLong,
		TypeSummaryTitlesTooShort,
		TypeSummaryTitlesH1Duplicates,
		TypeSummaryTitlesSeveralTags,
		TypeSummaryMetaDescriptionsEmpty,
		TypeSummaryMetaDescriptionsDuplicates,
		TypeSummaryMetaDescriptionsTooLong,
		TypeSummaryMetaDescriptionsTooShort,
		TypeSummaryMetaDescriptionsSeveralTags,
		TypeSummaryMetaKeyWordsEmpty,
		TypeSummaryMetaKeyWordsDuplicates,
		TypeSummaryMetaKeyWordsSeveralTags,
		TypeSummaryH1Missing,
		TypeSummaryH1Duplicates,
		TypeSummaryH1TooLong,
		TypeSummaryH1SeveralTags,
		TypeSummaryH2Missing,
		TypeSummaryH2Duplicates,
		TypeSummaryH2TooLong,
		TypeSummaryH2SeveralTags,
		TypeSummaryImagesOver100KB,
		TypeSummaryImagesAltMissing,
		TypeSummaryImagesAltTooLong
	};

	enum Orientation
	{
		OrientationHeaderAtTop,
		OrientationHeaderAtLeft
	};

	Type accessorType;
	Orientation orientation;

	ModelDataAccessorFactoryParams(Type accessorType, Orientation orientation = OrientationHeaderAtTop)
		: accessorType(accessorType)
		, orientation(orientation)
	{
	}

	bool isValid() const
	{
		return accessorType != TypeInvalid;
	}

	// TODO: add params

};

class ModelDataAccessorFactory
{

public:
	std::unique_ptr<IModelDataAccessor> getModelDataAccessor(const ModelDataAccessorFactoryParams& params);

};

}