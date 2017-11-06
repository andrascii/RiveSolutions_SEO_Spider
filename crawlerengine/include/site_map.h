#pragma once

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
	QDateTime lastModifiedDate;
	double priorityLevelSettings[6];
	SitemapChangeFreq changeFreqLevelSettings[6];
};


class SiteMap
{
public:
	

	SiteMap();
	QString xml(const SequencedStorage& crawledPages, const SiteMapSettings& settings);

private:
	struct Node;
	using NodePtr = std::shared_ptr<Node>;
	struct Node
	{
		QString path;
		QUrl originalUrl;
		std::map<QString, NodePtr> children;
	};


	void clear();
	QString xml() const;
	NodePtr root() const;
	void addUrl(const QUrl& url);
	NodePtr createNodeIfRequired(const QUrl& url);
	NodePtr createHostNodeIfRequired(const QUrl& url);

private:
	NodePtr m_root;
};

}