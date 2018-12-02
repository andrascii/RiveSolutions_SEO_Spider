#include "h_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{


HParser::HParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
{
}

void HParser::parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage)
{
	Q_UNUSED(headers);

	if (parsedPage->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseH1(parsedPage);
	parseH2(parsedPage);
}

void HParser::parseH1(ParsedPagePtr& parsedPage) noexcept
{
	std::vector<IHtmlNodeCountedPtr> h1Tags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdH1);

	std::vector<QByteArray> h1TagsValues;
	h1TagsValues.reserve(h1Tags.size());

	for (const IHtmlNodeCountedPtr& h1Tag : h1Tags)
	{
		h1TagsValues.push_back(h1Tag->text().trimmed());
	}

	h1TagsValues.erase(std::remove_if(h1TagsValues.begin(), h1TagsValues.end(),
		[](const QByteArray& h1Tag) { return h1Tag.isEmpty(); }), h1TagsValues.end());

	if (!h1TagsValues.empty())
	{
		parsedPage->firstH1 = h1TagsValues.front();
	}

	if (h1TagsValues.size() > 1)
	{
		parsedPage->secondH1 = h1TagsValues[1];
		parsedPage->hasSeveralH1Tags = true;
	}
}

void HParser::parseH2(ParsedPagePtr& parsedPage) noexcept
{
	std::vector<IHtmlNodeCountedPtr> h2Tags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdH2);

	std::vector<QByteArray> h2TagsValues;
	h2TagsValues.reserve(h2Tags.size());

	for (const IHtmlNodeCountedPtr& h2Tag : h2Tags)
	{
		h2TagsValues.push_back(h2Tag->text().trimmed());
	}

	h2TagsValues.erase(std::remove_if(h2TagsValues.begin(), h2TagsValues.end(),
		[](const QByteArray& h2Tag) { return h2Tag.isEmpty(); }), h2TagsValues.end());

	if (!h2TagsValues.empty())
	{
		parsedPage->firstH2 = h2TagsValues.front();
	}

	if (h2TagsValues.size() > 1)
	{
		parsedPage->secondH2 = h2TagsValues[1];

		std::set<QString> set;

		for (const QByteArray& h2TagValue : h2TagsValues)
		{
			set.insert(h2TagValue);
		}

		parsedPage->hasSeveralEqualH2Tags = set.size() != static_cast<int>(h2TagsValues.size());
	}
}

}
