#pragma once

#include "storage_type.h"

namespace SeoSpider
{

enum class AuditGroup
{
	//
	// Problem groups
	//
	LinkAuditGroup,
	OnPageAuditGroup,
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
	CrawlerEngine::StorageType storageType;
	QString storageTypeDescriptionName;
};

struct DCStorageGroupDescription
{
	QString name;
	AuditGroup group;
	std::vector<DCStorageDescription> descriptions;
};

using DCStorageGroupDescriptionPtr = std::shared_ptr<DCStorageGroupDescription>;

class DataCollectionGroupsFactory
{
public:
	DCStorageGroupDescriptionPtr create(AuditGroup group) const;
};

}