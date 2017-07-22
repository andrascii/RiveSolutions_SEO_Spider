#include "html_page_h_parser.h"
#include "gumbo_parsing_helpers.h"

namespace QuickieWebBot
{

void HtmlPageHParser::parse(GumboOutput* output, PageInfoPtr& pageInfo) noexcept
{
	GumboNode* body = GumboParsingHelpers::findSubNode(output->root, GUMBO_TAG_BODY);
	assert(body->type == GUMBO_NODE_ELEMENT && body->v.element.tag == GUMBO_TAG_BODY);

	recurseSearch(body, GUMBO_TAG_H1, m_h1Tags);
	recurseSearch(body, GUMBO_TAG_H2, m_h2Tags);

	if (m_h1Tags.size())
	{
		pageInfo->firstH1 = m_h1Tags[0];

		if (m_h1Tags.size() >= 2)
		{
			pageInfo->secondH1 = m_h1Tags[1];
		}
	}

	if (m_h2Tags.size())
	{
		pageInfo->firstH2 = m_h2Tags[0];

		if (m_h2Tags.size() >= 2)
		{
			pageInfo->secondH2 = m_h2Tags[1];
		}
	}

	m_h1Tags.clear();
	m_h2Tags.clear();
}

void HtmlPageHParser::recurseSearch(const GumboNode* node, GumboTag tag, std::vector<QString>& container) noexcept
{
	if (!node || (node && node->type != GUMBO_NODE_ELEMENT))
	{
		return;
	}

	if (node->v.element.tag == tag && node->v.element.children.length > 0)
	{
		container.push_back(GumboParsingHelpers::nodeText(static_cast<GumboNode*>(node->v.element.children.data[0])));
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned int i = 0; i < children->length; ++i)
	{
		recurseSearch(static_cast<const GumboNode*>(children->data[i]), tag, container);
	}
}

}