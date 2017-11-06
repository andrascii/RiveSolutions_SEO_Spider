#pragma once
#include "parsed_page.h"

namespace CrawlerEngine
{

class SequencedStorage;

enum SiteMapIncludeSettings
{
	IncludeNoIndexPages = 1,
	IncludeCanonicalised = 1 << 1,
	IncludePaginatedUrls = 1 << 2,
	IncludePDFs = 1 << 3,
	IncludeLastModTag = 1 << 4,
	IncludePriorityTag = 1 << 5,
	IncludeChangeFreqTag = 1 << 6,
	IncludeImages = 1 << 7,
	IncludeNoIndexImages = 1 << 8
};

Q_DECLARE_FLAGS(SiteMapIncludeSettingsFlags, SiteMapIncludeSettings);

enum class SiteMapLastModTagMode
{
	Calculated,
	Manual
};

enum class SiteMapChangeFreqTagMode
{
	CalculatedFromLastModifiedHeader,
	UseLevelSettings
};

enum class SitemapChangeFreq
{
	Always,
	Hourly,
	Daily,
	Weekly,
	Monthly,
	Yearly,
	Never
};

struct SiteMapSettings
{
	SiteMapIncludeSettingsFlags flags;
	SiteMapLastModTagMode lastModifiedMode;
	QDateTime lastModifiedDate;
	double priorityLevelSettings[6];
	SiteMapChangeFreqTagMode changeFreqMode;
	SitemapChangeFreq changeFreqLevelSettings[6];
};


class SiteMap
{
public:
	QString xml(const SequencedStorage& crawledPages, const SiteMapSettings& settings) const;

private:
	bool includeInSiteMap(const ParsedPage* page, const SiteMapSettings& settings) const;
	bool discardByNoIndex(const ParsedPage* page, const SiteMapSettings& settings) const;
	bool discardByResourceType(const ParsedPage* page, const SiteMapSettings& settings) const;
	bool discardByStatusCode(const ParsedPage* page) const;
	
};

}
