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

		QString contentAttributeValue = 
			QString(contentAttribute->value).trimmed().remove(QChar('\n'), Qt::CaseInsensitive);

		GumboAttribute* metaHttpEquivAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "http-equiv");
		GumboAttribute* metaNameAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "name");

		assert(metaHttpEquivAttribute && !metaNameAttribute ||
			!metaHttpEquivAttribute && metaNameAttribute ||
			!metaHttpEquivAttribute && !metaNameAttribute);

		if (metaHttpEquivAttribute)
		{
			QString attributeValue = QString(metaHttpEquivAttribute->value).toLower().trimmed();

			if (attributeValue == "content-type")
			{
				pageInfo->content = contentAttributeValue;
			}

			if (attributeValue == "refresh")
			{
				pageInfo->metaRefresh = contentAttributeValue;
			}
		}

		if (metaNameAttribute)
		{
			QString attributeValue = QString(metaNameAttribute->value).toLower();

			if (attributeValue == "description")
			{
				pageInfo->metaDescription = contentAttributeValue;
			}

			if (attributeValue == "keywords")
			{
				pageInfo->metaKeywords = contentAttributeValue;
			}

			if (attributeValue == "robots")
			{
				pageInfo->metaRobots = contentAttributeValue;
			}
		}
	}
}

}