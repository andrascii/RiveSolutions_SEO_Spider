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

	static const QString priorityOpenTag = QString("<priority>");
	static const QString priorityCloseTag = QString("</priority>");

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

QString SiteMap::xml(const ISequencedStorage& crawledPages, const SiteMapSettings& settings) const
{
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
		result = result % tab % tab % locOpenTag %  page->url.toDisplayString(CustomUrl::FullyEncoded) % locCloseTag % endLine;

		if (settings.flags.testFlag(IncludeLastModTag))
		{
			const QString date = settings.lastModifiedMode == SiteMapLastModTagMode::Manual
				? formatDate(settings.lastModifiedDate) : responseDate(page);

			if (!date.isEmpty())
			{
				result = result % tab % tab % lastModOpenTag %  date % lastModCloseTag % endLine;
			}
		}

		const int level = pageLevel(page);

		if (includePriority)
		{
			const double priority = settings.priorityLevelSettings[level - 1];
			DEBUG_ASSERT(priority > 0.0 && priority <= 1.0)
			const QString prioritySr = QString::number(priority);
			result = result % tab % tab % priorityOpenTag % prioritySr % priorityCloseTag % endLine;
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
		!discardByNoIndex(page, settings) &&
		!discardByCanonicalNextPrev(page, settings);
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
		return discardImageByNoImageIndex(page, settings);
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

bool SiteMap::discardByCanonicalNextPrev(const ParsedPage* page, const SiteMapSettings& settings) const
{
	bool canonical = false;
	bool nextOrPrev = false;
	bool other = page->linksToThisPage.empty();

	for (const ResourceLink& link : page->linksToThisPage)
	{
		switch (link.resourceSource)
		{
		case ResourceSource::SourceTagLinkRelCanonical:
			canonical = true;
			break;
		case ResourceSource::SourceTagLinkRelNext:
		case ResourceSource::SourceTagLinkRelPrev:
			nextOrPrev = true;
			break;
		default:
			other = true;
		}

		if (other)
		{
			break;
		}

		if (nextOrPrev && canonical)
		{
			break;
		}
	}

	if (other)
	{
		return false;
	}

	if (settings.flags.testFlag(IncludeCanonicalised) && canonical)
	{
		return false;
	}
	
	if (settings.flags.testFlag(IncludePaginatedUrls) && nextOrPrev)
	{
		return false;
	}

	return true;
}

bool SiteMap::discardImageByNoImageIndex(const ParsedPage* page, const SiteMapSettings& settings) const
{
	if (settings.flags.testFlag(IncludeNoIndexImages))
	{
		return false;
	}

	auto it = page->metaRobotsFlags.find(UserAgentType::AnyBot);
	if (it != page->metaRobotsFlags.end() && it->second.testFlag(MetaRobotsNoImageIndex))
	{
		return true;
	}

	if (page->linksToThisPage.empty())
	{
		return false;
	}

	for (const ResourceLink& link : page->linksToThisPage)
	{
		//DEBUG_ASSERT(!link.resource.expired());

		if (link.resource.expired())
		{
			continue;
		}

		const MetaRobotsFlagsSet& pageMetaFlags = link.resource.lock()->metaRobotsFlags;
		auto it = pageMetaFlags.find(UserAgentType::AnyBot);
		if (it == pageMetaFlags.end() || !it->second.testFlag(MetaRobotsNoImageIndex))
		{
			// do not discard image if at least one page that 
			// contains this image doesn't have noimageindex
			return false;
		}
	}

	return true;
}

QString SiteMap::responseDate(const ParsedPage* page) const
{
	return formatDate(page->lastModifiedDate.isValid() ? page->lastModifiedDate : QDateTime::currentDateTime());
}

QString SiteMap::formatDate(const QDateTime& dateTime) const
{
	return dateTime.isValid() ? dateTime.toString(QString("yyyy-MM-dd")) : QString();
}

SiteMap::PageLevel SiteMap::pageLevel(const ParsedPage* page) const
{
	return page->pageLevel < static_cast<int>(Level5Plus) ? static_cast<PageLevel>(page->pageLevel) : Level5Plus;
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
