#include "html_page_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
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


void HtmlPageParser::addPageRawParser(std::shared_ptr<IPageParser> pageRawParser) noexcept
{
	m_parserPack.push_back(std::move(pageRawParser));
}

void HtmlPageParser::parsePage(const QByteArray& htmlPage, PageRawPtr& pageRaw) noexcept
{
	//
	// TODO: optimize this code because for now we need double parsing of the same page
	//


	QByteArray decodedHtmlPage = decodeHtmlPage(htmlPage);
	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, decodedHtmlPage);

	for (const std::shared_ptr<IPageParser>& parser : m_parserPack)
	{
		parser->parse(gumboOutput.output(), pageRaw);
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
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);
		parsePageUrlList(child);
	}
}


QByteArray HtmlPageParser::identifyHtmlPageContentType(const QByteArray& htmlPage) const noexcept
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
				return contentAttribute->value;
			}
		}
	}

	return QByteArray();
}

QByteArray HtmlPageParser::decodeHtmlPage(const QByteArray& htmlPage) const noexcept
{
	QByteArray decodedHtmlPage = htmlPage;
	QByteArray contentType = identifyHtmlPageContentType(htmlPage);

	QByteArray charset = contentType.right(contentType.size() - contentType.lastIndexOf("=") - 1);

	if (charset.isEmpty())
	{
		QTextCodec* codecForHtml = QTextCodec::codecForHtml(decodedHtmlPage.data());

		if (codecForHtml)
		{
			decodedHtmlPage = codecForHtml->toUnicode(decodedHtmlPage).toStdString().data();
		}
		else
		{
			ERRORLOG << "Cannot identify page encoding";
		}
	}
	else
	{
		QTextCodec* codecForCharset = QTextCodec::codecForName(charset);

		if (codecForCharset)
		{
			decodedHtmlPage = codecForCharset->toUnicode(decodedHtmlPage.data()).toStdString().data();
		}
		else
		{
			ERRORLOG << "Cannot find QTextCodec for page encoding";
			ERRORLOG << "content-type:" << contentType;
			ERRORLOG << "charset:" << charset;
		}
	}

	return decodedHtmlPage;
}

}