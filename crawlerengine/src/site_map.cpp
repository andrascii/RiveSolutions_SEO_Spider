#include "site_map.h"
#include "page_parser_helpers.h"
#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

QString SiteMap::xml(const SequencedStorage& crawledPages, const SiteMapSettings& settings) const
{
	Q_UNUSED(settings); // TODO: use settings
	
	const QString header = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	const QString endLine = QString("\r\n");
	const QString tab = QString("\t");

	const QString urlSetOpenTag = QString("<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">");
	const QString urlSetCloseTag = QString("</urlset>");

	const QString urlOpenTag = QString("<url>");
	const QString urlCloseTag = QString("</url>");

	const QString locOpenTag = QString("<loc>");
	const QString locCloseTag = QString("</loc>");

	QString result = header % endLine % urlSetOpenTag % endLine;

	for (int i = 0; i < crawledPages.size(); i++)
	{
		const ParsedPage* page = crawledPages[i];
		if (!includeInSiteMap(page, settings))
		{
			continue;
		}

		result = result % tab % urlOpenTag % endLine % tab % tab % locOpenTag;
		result = result %  page->url.toDisplayString(QUrl::FullyEncoded);
		result = result % locCloseTag % endLine;

		if (settings.flags.testFlag(IncludeLastModTag))
		{
			// TODO: implement
		}

		if (settings.flags.testFlag(IncludePriorityTag))
		{
			// TODO: implement
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

}
