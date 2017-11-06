#include "site_map.h"
#include <QStringBuilder>
#include "page_parser_helpers.h"
#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

SiteMap::SiteMap()
{
}

void SiteMap::clear()
{
	// root can have several host children like sample.com, another.sample.com
	m_root.reset(new Node);
}

QString SiteMap::xml(const SequencedStorage& crawledPages, const SiteMapSettings& settings)
{
	Q_UNUSED(settings); // TODO: use settings
	clear();

	for (int i = 0; i < crawledPages.size(); i++)
	{
		addUrl(crawledPages[i]->url);
	}

	return xml();
}

void SiteMap::addUrl(const QUrl& url)
{
	ASSERT(!url.isRelative());
	ASSERT(PageParserHelpers::isHttpOrHttpsScheme(url));

	createNodeIfRequired(url);
}

QString SiteMap::xml() const
{
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


	std::function<void(const NodePtr&)> traverse = 
		[&traverse, &result, &tab, &endLine, &urlOpenTag, &urlCloseTag, &locOpenTag, &locCloseTag](const NodePtr& node)
	{
		if (node->children.empty())
		{
			result = result % tab % urlOpenTag % endLine % tab % tab % locOpenTag;
			result = result % node->originalUrl.toDisplayString(QUrl::FullyEncoded);
			result = result % locCloseTag % endLine % tab % urlCloseTag % endLine;
		}

		for (const std::pair<const QString, NodePtr>& childNode : node->children)
		{
			traverse(childNode.second);
		}
	};

	traverse(m_root);

	result = result % urlSetCloseTag;

	return result;
}

SiteMap::NodePtr SiteMap::createNodeIfRequired(const QUrl& url)
{
	NodePtr currentNode = createHostNodeIfRequired(url);
	
	const QString path = url.path();
	const QStringList pathParts = path.split(QChar('/'), QString::SkipEmptyParts);
	int index = 0;

	foreach(const QString& pathPart, pathParts)
	{
		auto it = currentNode->children.find(pathPart.toLower());
		if (it != currentNode->children.end())
		{
			currentNode = it->second;
		}
		else
		{
			NodePtr newNode = std::make_shared<Node>(Node{ pathPart });
			m_root->children[pathPart.toLower()] = newNode;

			if (index == pathParts.size() - 1)
			{
				newNode->originalUrl = url;
			}

			currentNode = newNode;
		}
	}

	return currentNode;
}

SiteMap::NodePtr SiteMap::createHostNodeIfRequired(const QUrl& url)
{
	QString host = url.host();

	if (host.toLower().startsWith(QString("www")))
	{
		host = host.mid(3); // ???
	}

	auto hostNodeIt = m_root->children.find(host.toLower());
	NodePtr hostNode;

	if (hostNodeIt != m_root->children.end())
	{
		hostNode = hostNodeIt->second;
	}
	else
	{
		hostNode = std::make_shared<Node>(Node{ host });
		m_root->children[host.toLower()] = hostNode;
	}

	return hostNode;
}

}
