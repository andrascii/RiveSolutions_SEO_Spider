#include "xpath_parser.h"
#include "html_node_model.h"

namespace
{

QStringList s_prefixesToRemove{ "/html", "//html" };

QStringList s_availableSuffixes{ "string()" }; // TODO: addd more

QString correctedXPath(const QString& xPath)
{
	QString result = xPath.trimmed();
	for (const QString& prefixToRemove : s_prefixesToRemove)
	{
		if (result.startsWith(prefixToRemove))
		{
			result = result.mid(prefixToRemove.size());
		}
	}

	bool hasAvailableSuffix = false;
	for (const QString& availableSuffix : s_availableSuffixes)
	{
		if (result.endsWith(availableSuffix))
		{
			hasAvailableSuffix = true;
			break;
		}
	}

	if (!hasAvailableSuffix)
	{
		if (result.endsWith("/"))
		{
			result += "string()";
		}
		else
		{
			result += "/string()";
		}
	}

	return "$doc" + result;
}

QString evaluateXPath(const QString& xpath, CrawlerEngine::IHtmlParser* parser)
{
	QXmlQuery xmlQuery(QXmlQuery::XQuery10);

	CrawlerEngine::HtmlNodeModel model(xmlQuery.namePool(), parser);

	// TODO: somehove reuse model
	QXmlNodeModelIndex startNode = model.rootIndex();
	xmlQuery.bindVariable("doc", startNode);
	xmlQuery.setQuery(xpath, QUrl("https://google.com"));

	QString result;
	xmlQuery.evaluateTo(&result);
	return result.trimmed();
}
		
}

namespace CrawlerEngine
{

XPathParser::XPathParser(IHtmlParser* htmlParser, const CrawlerOptionsData& options)
	: m_htmlParser(htmlParser)
	, m_options(options)
{
	int index = 0;
	for (auto xpath : options.extractorRules)
	{
		// TODO: check if it's xpath extractor rule
		if (!xpath.isEmpty())
		{
			m_xPaths.insert(index, correctedXPath(xpath));
		}
		index += 1;
	}
}

void XPathParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	for (auto it = m_xPaths.keyBegin(); it != m_xPaths.keyEnd(); ++it)
	{
		QString result = evaluateXPath(m_xPaths[*it], m_htmlParser);
		page->dataExtractions.insert(*it, result);
	}
}

}


