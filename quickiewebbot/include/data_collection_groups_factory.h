#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

// DC - DataCollection

struct DCStorageDescription
{
	WebCrawler::DataCollection::StorageType storageType;
	QString storageTypeDescriptionName;
};

struct DCStorageGroupDescription
{
	QString name;
	QVector<DCStorageDescription> descriptions;
};

enum class ProblemAuditGroups
{
	LinkProblems,
	TitleProblems,
	MetaDescriptionProblems,
	MetaKeywordProblems,
	H1Problems,
	H2Problems,
	ImageProblems
};

using DCStorageDescriptionPtr = std::shared_ptr<DCStorageGroupDescription>;

class DataCollectionGroupsFactory
{
public:
	DCStorageDescriptionPtr create(WebCrawler::DataCollection* dataCollection, ProblemAuditGroups problemsGroup);
};

}