#include "html_page_meta_parser.h"
#include "gumbo_parsing_helpers.h"

namespace QuickieWebBot
{

void HtmlPageMetaParser::parse(GumboOutput* output, PageInfoPtr& pageInfo) noexcept
{
	GumboNode* head = GumboParsingHelpers::findSubNode(output->root, GUMBO_TAG_HEAD);

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
		GumboAttribute* metaNameAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "name");

		assert(metaHttpEquivAttribute && !metaNameAttribute ||
			!metaHttpEquivAttribute && metaNameAttribute ||
			!metaHttpEquivAttribute && !metaNameAttribute);

		if (metaHttpEquivAttribute)
		{
			QString attributeValue = QString(metaHttpEquivAttribute->value).toLower();

			if (attributeValue == "content-type")
			{
				pageInfo->content = contentAttribute->value;
			}

			if (attributeValue == "refresh")
			{
				pageInfo->metaRefresh = contentAttribute->value;
			}
		}

		if (metaNameAttribute)
		{
			QString attributeValue = QString(metaNameAttribute->value).toLower();

			if (attributeValue == "description")
			{
				pageInfo->metaDescription = contentAttribute->value;
			}

			if (attributeValue == "keywords")
			{
				pageInfo->metaKeywords = contentAttribute->value;
			}

			if (attributeValue == "robots")
			{
				pageInfo->metaRobots = contentAttribute->value;
			}
		}
	}
}

}