#include "frames_detector_parser.h"
#include "page_parser_helpers.h"
#include "gumbo_parsing_helpers.h"
#include "parsed_page.h"

namespace CrawlerEngine
{

void FramesDetectorParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	auto findPredicate = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			(node->v.element.tag == GUMBO_TAG_FRAME || node->v.element.tag == GUMBO_TAG_FRAMESET) &&
			node->parent;
	};

	const std::vector<const GumboNode*> frames = GumboParsingHelpers::findNodesRecursive(output->root, findPredicate);

	page->hasFrames = !frames.empty();
}

}