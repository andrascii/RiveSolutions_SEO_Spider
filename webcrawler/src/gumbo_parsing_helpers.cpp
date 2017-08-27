#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

GumboOutputCreatorDestroyerGuard::GumboOutputCreatorDestroyerGuard(const GumboOptions* options, const QByteArray& htmlPage)
	: m_gumboOptions(options)
	, m_gumboOutput(gumbo_parse_with_options(options, htmlPage.data(), htmlPage.size()))
{
}

GumboOutputCreatorDestroyerGuard::~GumboOutputCreatorDestroyerGuard()
{
	gumbo_destroy_output(m_gumboOptions, m_gumboOutput);
}

const GumboOutput* GumboOutputCreatorDestroyerGuard::output() const noexcept
{
	return m_gumboOutput;
}

GumboOutput* GumboOutputCreatorDestroyerGuard::output() noexcept
{
	return m_gumboOutput;
}


GumboNode* GumboParsingHelpers::findSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent) noexcept
{
	assert(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	GumboNode* searchingNode = nullptr;

	assert(startIndexWhithinParent <= children->length);

	for (unsigned i = startIndexWhithinParent; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			searchingNode = child;
			break;
		}
	}

	return searchingNode;
}

QByteArray GumboParsingHelpers::nodeText(const GumboNode* node) noexcept
{
	return node->v.text.text;
}

std::vector<GumboNode*> GumboParsingHelpers::subNodes(const GumboNode* node, GumboTag tag) noexcept
{
	assert(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	std::vector<GumboNode*> nodes;

	for (unsigned i = 0; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			nodes.push_back(child);
		}
	}

	return nodes;
}

unsigned GumboParsingHelpers::countChildren(const GumboNode* node, GumboTag tag) noexcept
{
	assert(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	unsigned counter = 0;

	for (unsigned i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			++counter;
		}
	}

	return counter;
}

QByteArray GumboParsingHelpers::cutAllTagsFromNode(const GumboNode* node) noexcept
{
	QByteArray text;
	cutAllTagsFromNodeHelper(node, text);

	return text;
}


QByteArray GumboParsingHelpers::identifyHtmlPageContentType(const QByteArray& htmlPage) noexcept
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


QByteArray GumboParsingHelpers::decodeHtmlPage(const QByteArray& htmlPage) noexcept
{
	QByteArray decodedHtmlPage = htmlPage;
	QByteArray contentType = GumboParsingHelpers::identifyHtmlPageContentType(htmlPage);

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

std::vector<QUrl> GumboParsingHelpers::parsePageUrlList(const GumboNode* node) noexcept
{
	std::vector<QUrl> result;


	if (!node || (node && node->type != GUMBO_NODE_ELEMENT))
	{
		return result;
	}

	GumboAttribute* href = nullptr;

	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href")))
	{
		result.push_back(QUrl(href->value));
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned int i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);
		std::vector<QUrl> childResult = parsePageUrlList(child);

		result.insert(std::begin(result), std::end(childResult), std::end(childResult));
	}

	return result;
}

void GumboParsingHelpers::cutAllTagsFromNodeHelper(const GumboNode* node, QByteArray& result) noexcept
{
	if (!node)
	{
		return;
	}

	if (node->type == GUMBO_NODE_TEXT)
	{
		//result += nodeText(node) % " ";
		result += nodeText(node) + " ";
		return;
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT || child->type == GUMBO_NODE_TEXT)
		{
			cutAllTagsFromNodeHelper(child, result);
		}
	}
}

}