#include "html_page_parser.h"
#include "gumbo_parsing_helpers.h"

namespace QuickieWebBot
{

class GumboOutputCreatorDestroyerGuard
{
public:
	GumboOutputCreatorDestroyerGuard(const GumboOptions* options, const QByteArray& htmlPage)
		: m_gumboOptions(options)
		, m_gumboOutput(gumbo_parse_with_options(options, htmlPage.data(), htmlPage.size()))
	{
	}

	~GumboOutputCreatorDestroyerGuard()
	{
		gumbo_destroy_output(m_gumboOptions, m_gumboOutput);
	}

	GumboOutputCreatorDestroyerGuard(const GumboOutputCreatorDestroyerGuard&) = delete;
	GumboOutputCreatorDestroyerGuard(GumboOutputCreatorDestroyerGuard&&) = delete;
	GumboOutputCreatorDestroyerGuard& operator=(const GumboOutputCreatorDestroyerGuard&) = delete;
	GumboOutputCreatorDestroyerGuard& operator=(GumboOutputCreatorDestroyerGuard&&) = delete;

	const GumboOutput* output() const noexcept
	{
		return m_gumboOutput;
	}

	GumboOutput* output() noexcept
	{
		return m_gumboOutput;
	}

private:
	const GumboOptions* m_gumboOptions;
	GumboOutput* m_gumboOutput;
};


void HtmlPageParser::addPageInfoParser(std::shared_ptr<IPageParser> pageInfoParser) noexcept
{
	m_parserPack.push_back(std::move(pageInfoParser));
}

void HtmlPageParser::parsePage(const QByteArray& htmlPage, PageInfoPtr& pageInfo) noexcept
{
	//
	// TODO: optimize this code because for now we need double parsing of the same page
	//

	QByteArray decodedHtmlPage = htmlPage;
	QByteArray charset = identifyHtmlPageCharset(htmlPage);

	if (charset.isEmpty())
	{
		decodedHtmlPage = QTextCodec::codecForHtml(decodedHtmlPage.data())->toUnicode(decodedHtmlPage).toStdString().data();
	}
	else
	{
		decodedHtmlPage = QTextCodec::codecForName(charset)->toUnicode(decodedHtmlPage.data()).toStdString().data();
	}

	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, decodedHtmlPage);

	for (const std::shared_ptr<IPageParser>& parser : m_parserPack)
	{
		parser->parse(gumboOutput.output(), pageInfo);
	}

	m_pageUrlList.clear();

	parsePageUrlList(gumboOutput.output()->root);
}

const std::vector<QUrl>& HtmlPageParser::pageUrlList() const noexcept
{
	return m_pageUrlList;
}

void HtmlPageParser::parsePageUrlList(const GumboNode* node) noexcept
{
	if (!node || (node && node->type != GUMBO_NODE_ELEMENT))
	{
		return;
	}

	GumboAttribute* href = nullptr;

	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href")))
	{
		m_pageUrlList.push_back(QUrl(href->value));
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned int i = 0; i < children->length; ++i)
	{
		parsePageUrlList(static_cast<const GumboNode*>(children->data[i]));
	}
}


QByteArray HtmlPageParser::identifyHtmlPageCharset(const QByteArray& htmlPage) const noexcept
{
	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, htmlPage);

	GumboNode* head = GumboParsingHelpers::findSubNode(gumboOutput.output()->root, GUMBO_TAG_HEAD);
	assert(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	std::vector<GumboNode*> metaTags = GumboParsingHelpers::subNodes(head, GUMBO_TAG_META);

	for (unsigned i = 0; i < metaTags.size(); ++i)
	{
		GumboAttribute* contentAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "content");

		if (!contentAttribute)
		{
			continue;
		}

		GumboAttribute* metaHttpEquivAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "http-equiv");

		if (metaHttpEquivAttribute)
		{
			QString attributeValue = QString(metaHttpEquivAttribute->value).toLower();

			if (attributeValue == "content-type")
			{
				QString contentType = contentAttribute->value;
				contentType.right(contentType.size() - contentType.lastIndexOf("="));

				return contentType.right(contentType.size() - contentType.lastIndexOf("=") - 1).toUtf8();
			}
		}
	}

	return QByteArray();
}

}