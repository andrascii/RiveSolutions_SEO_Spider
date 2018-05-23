#include "frames_detector_parser.h"
#include "page_parser_helpers.h"
#include "ihtml_parser.h"
#include "parsed_page.h"

namespace CrawlerEngine
{

FramesDetectorParser::FramesDetectorParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
{
}

void FramesDetectorParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);

// 	auto findPredicate = [](const GumboNode* node)
// 	{
// 		return node &&
// 			node->type == GUMBO_NODE_ELEMENT &&
// 			(node->v.element.tag == GUMBO_TAG_FRAME || node->v.element.tag == GUMBO_TAG_FRAMESET) &&
// 			node->parent;
// 	};
// 
// 	const std::vector<const GumboNode*> frames = GumboParsingHelpers::findNodesRecursive(output->root, findPredicate);
// 
// 	page->hasFrames = !frames.empty();

	std::vector<IHtmlNodeSharedPtr> frames = m_htmlParser->matchNodesInDepth([](const IHtmlNode& node)
	{ 
		return node.type() == IHtmlNode::NodeTypeElement &&
			(node.tagId() == IHtmlNode::TagIdFrame || node.tagId() == IHtmlNode::TagIdFrameSet);
	});

	page->hasFrames = !frames.empty();
}

}