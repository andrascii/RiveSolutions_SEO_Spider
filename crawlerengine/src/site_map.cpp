#include "site_map.h"
#include "page_parser_helpers.h"
#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

namespace
{
	static const QString header = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	static const QString endLine = QString("\r\n");
	static const QString tab = QString("\t");

	static const QString urlSetOpenTag = QString("<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">");
	static const QString urlSetCloseTag = QString("</urlset>");

	static const QString urlOpenTag = QString("<url>");
	static const QString urlCloseTag = QString("</url>");

	static const QString locOpenTag = QString("<loc>");
	static const QString locCloseTag = QString("</loc>");

	static const QString lastModOpenTag = QString("<lastmod>");
	static const QString lastModCloseTag = QString("</lastmod>");

	static const QString changeFreqOpenTag = QString("<changefreq>");
	static const QString changeFreqCloseTag = QString("</changefreq>");

	static const QMap<SitemapChangeFreq, QString> changeFrequencies
	{
		{ SitemapChangeFreq::Always, QString("always") },
		{ SitemapChangeFreq::Hourly, QString("hourly") },
		{ SitemapChangeFreq::Daily, QString("daily") },
		{ SitemapChangeFreq::Weekly, QString("weekly") },
		{ SitemapChangeFreq::Monthly, QString("monthly") },
		{ SitemapChangeFreq::Yearly, QString("yearly") },
		{ SitemapChangeFreq::Never, QString("never") }
	};
}

QString SiteMap::xml(const SequencedStorage& crawledPages, const SiteMapSettings& settings) const
{
	Q_UNUSED(settings); // TODO: use settings
	
	const bool includePriority = settings.flags.testFlag(IncludePriorityTag);
	const bool includeChangeFreq = settings.flags.testFlag(IncludeChangeFreqTag);
	const QDateTime now = QDateTime::currentDateTime();

	QString result = header % endLine % urlSetOpenTag % endLine;

	for (int i = 0; i < crawledPages.size(); i++)
	{
		const ParsedPage* page = crawledPages[i];
		if (!includeInSiteMap(page, settings))
		{
			continue;
		}

		result = result % tab % urlOpenTag % endLine;
		result = result % tab % tab % locOpenTag %  page->url.toDisplayString(QUrl::FullyEncoded) % locCloseTag % endLine;

		if (settings.flags.testFlag(IncludeLastModTag))
		{
			const QString date = settings.lastModifiedMode == SiteMapLastModTagMode::Manual
				? formatDate(settings.lastModifiedDate) : responseDate(page);

			if (!date.isEmpty())
			{
				result = result % tab % tab % lastModOpenTag %  date % lastModCloseTag % endLine;
			}
		}

		const int level = includePriority ||
			includeChangeFreq && settings.changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings
			? pageLevel(crawledPages[0], page) : 0;


		if (includePriority)
		{
			// TODO: implement
		}

		if (includeChangeFreq)
		{
			QString frequencyStr;
			if (settings.changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings)
			{
				const SitemapChangeFreq frequency = settings.changeFreqLevelSettings[level - 1];
				ASSERT(frequency >= SitemapChangeFreq::Always && frequency <= SitemapChangeFreq::Never);
				frequencyStr = changeFrequencies.value(frequency, QString());
			}
			else if (page->lastModifiedDate.isValid())
			{
				const SitemapChangeFreq frequency = frequencyByDate(now, page->lastModifiedDate);
				frequencyStr = changeFrequencies.value(frequency, QString());
			}

			if (!frequencyStr.isEmpty())
			{
				result = result % tab % tab % changeFreqOpenTag % frequencyStr % changeFreqCloseTag % endLine;
			}
		}

		result = result % tab % urlCloseTag % endLine;
	}

	result = result % urlSetCloseTag;

	return result;
}

bool SiteMap::includeInSiteMap(const ParsedPage* page, const SiteMapSettings& settings) const
{
	return !discardByStatusCode(page) &&
		!discardByResourceType(page, settings) && 
		!discardByNoIndex(page, settings);
}

bool SiteMap::discardByNoIndex(const ParsedPage* page, const SiteMapSettings& settings) const
{
	if (settings.flags.testFlag(IncludeNoIndexPages))
	{
		return false;
	}

	auto it = page->metaRobotsFlags.find(UserAgentType::AnyBot);
	if (it == page->metaRobotsFlags.end())
	{
		return false;
	}

	return it->second.testFlag(MetaRobotsNoIndex) || it->second.testFlag(MetaRobotsNone);
}

bool SiteMap::discardByResourceType(const ParsedPage* page, const SiteMapSettings& settings) const
{
	const ResourceType resourceType = page->resourceType;

	if (resourceType == ResourceType::ResourceHtml)
	{
		return false;
	}

	if (settings.flags.testFlag(IncludeImages) && resourceType == ResourceType::ResourceImage)
	{
		return false;
	}

	if (settings.flags.testFlag(IncludePDFs) && 
		page->contentType.toLower().contains(QString("application/pdf")))
	{
		return false;
	}

	return true;
}

bool SiteMap::discardByStatusCode(const ParsedPage* page) const
{
	return page->statusCode != Common::StatusCode::Ok200;
}

QString SiteMap::responseDate(const ParsedPage* page) const
{
	return formatDate(page->lastModifiedDate.isValid() ? page->lastModifiedDate : QDateTime::currentDateTime());
}

QString SiteMap::formatDate(const QDateTime& dateTime) const
{
	return dateTime.isValid() ? dateTime.toString(QString("yyyy-MM-dd")) : QString();
}

SiteMap::PageLevel SiteMap::pageLevel(const ParsedPage* root,  const ParsedPage* page) const
{
	if (root == page)
	{
		return Level1;
	}

	return pageLevelImpl(1, root, page);
}

SiteMap::PageLevel SiteMap::pageLevelImpl(int currentLevel, const ParsedPage* root, const ParsedPage* page) const
{
	if (currentLevel == Level5Plus)
	{
		return static_cast<PageLevel>(currentLevel);
	}

	int result = Level5Plus;

	const size_t childCount = root->linksOnThisPage.size();
	for (size_t index = 0; index < childCount; ++index)
	{
		const ParsedPageWeakPtr& child = root->linksOnThisPage[index].resource;
		DEBUG_ASSERT(!child.expired());
		const ParsedPage* childPtr = child.lock().get();
		if (childPtr == page)
		{
			return static_cast<PageLevel>(currentLevel);
		}

		const PageLevel childResult = pageLevelImpl(currentLevel + 1, childPtr, page);
		if (childResult < result)
		{
			result = childResult;
		}

	}

	return static_cast<PageLevel>(result);
}

SitemapChangeFreq SiteMap::frequencyByDate(const QDateTime& now, const QDateTime& pageDate) const
{
	constexpr qint64 second = 1000;
	constexpr qint64 hour = second * 60 * 60;
	constexpr qint64 day = hour * 24;
	constexpr qint64 week = day * 7;
	constexpr qint64 month = day * 30;
	constexpr qint64 year = month * 12;

	static const QMap<qint64, SitemapChangeFreq> s_mapping
	{
		{ hour / 4, SitemapChangeFreq::Always },
		{ day / 2, SitemapChangeFreq::Hourly },
		{ week / 2, SitemapChangeFreq::Daily },
		{ month / 2, SitemapChangeFreq::Weekly },
		{ year / 2, SitemapChangeFreq::Monthly },
		{ year * 5, SitemapChangeFreq::Yearly }
	};

	ASSERT(now.isValid());
	ASSERT(pageDate.isValid());
	const qint64 msecs = pageDate.msecsTo(now);

	const QList<qint64> keys = s_mapping.keys();
	auto it = qLowerBound(keys, msecs);
	if (it == keys.cend())
	{
		return SitemapChangeFreq::Never;
	}

	return s_mapping[*it];
}

}
