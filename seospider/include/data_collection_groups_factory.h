#pragma once

#include "dc_storage_group_description.h"

namespace SeoSpider
{

class DataCollectionGroupsFactory
{
public:
	DCStorageGroupDescriptionPtr create(AuditGroup group) const;
	DCStorageGroupDescriptionPtr create(const QVector<CrawlerEngine::ICustomDataFeed*> dataFeeds) const;

private:
	std::vector<DCStorageDescription> createYandexMetricaDescriptions() const;
	std::vector<DCStorageDescription> createGoogleAnalyticsDescriptions() const;
};

}