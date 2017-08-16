#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

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