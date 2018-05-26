#include "title_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

TitleParser::TitleParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
}

void TitleParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page) noexcept
{
	Q_UNUSED(headers);

	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	std::vector<IHtmlNodeSharedPtr> titleTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdTitle);

	std::vector<QString> titleTagsValues;
	titleTagsValues.reserve(titleTags.size());

	for (const IHtmlNodeSharedPtr& titleTag : titleTags)
	{
		titleTagsValues.push_back(QString(titleTag->text()).trimmed().remove(m_regExp));
	}

	if (!titleTagsValues.empty())
	{
		page->title = titleTagsValues.front();
	}

	page->hasSeveralTitleTags = titleTagsValues.size() > 1;
}

}
