#include "stdafx.h"
#include "base_url_parser.h"
#include "ihtml_parser.h"

namespace
{
	const QString s_slash = "/";
	const QByteArray s_href = "href";
	const QString s_schemeSuffix = "://";
}

namespace CrawlerEngine
{

BaseUrlParser::BaseUrlParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
}

void BaseUrlParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);

	std::vector<IHtmlNodeCountedPtr> baseTagNodes = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdBase);
	IHtmlNodeCountedPtr baseTagNode = baseTagNodes.empty() ? nullptr : baseTagNodes[0];

	if (baseTagNode && baseTagNode->hasAttribute(s_href))
	{
		QString href = baseTagNode->attribute(s_href).remove(m_regExp);
		if (href == s_slash)
		{
			page->baseUrl = page->url.scheme() % s_schemeSuffix % page->url.host();
#ifdef QT_DEBUG
			page->baseUrl.canonizedUrlStr();
#endif
		}
		else
		{
			const Url url = href;
			if (url.isValid())
			{
				page->baseUrl = url;
			}
			else
			{
				WARNLOG << "On page:" << page->url.toDisplayString() << "found base tag but that tag has an invalid url address";

				page->baseUrl = page->url;
			}
		}
	}
	else
	{
		page->baseUrl = page->url;
	}
}

}