#pragma once

#include "unordered_data_collection.h"

namespace QuickieWebBot
{

enum class AuditGroup
{
	//
	// Problem groups
	//
	LinkAuditGroup,
	TitleAuditGroup,
	MetaDescriptionAuditGroup,
	MetaKeywordAuditGroup,
	H1AuditGroup,
	H2AuditGroup,
	ImageAuditGroup,

	//
	// Resources
	//
	InternalResourcesGroup,
	ExternalResourcesGroup
};

// DC - DataCollection

struct DCStorageDescription
{
	WebCrawler::StorageType storageType;
	QString storageTypeDescriptionName;
};

struct DCStorageGroupDescription
{
	QString name;
	AuditGroup group;
	QVector<DCStorageDescription> descriptions;
};

using DCStorageGroupDescriptionPtr = std::shared_ptr<DCStorageGroupDescription>;

class DataCollectionGroupsFactory
{
public:
	DCStorageGroupDescriptionPtr create(AuditGroup group) const;
};

}