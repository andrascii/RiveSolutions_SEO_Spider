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

	std::vector<IHtmlNodeCountedPtr> frames = m_htmlParser->matchNodesInDepth([](const IHtmlNode& node)
	{
		return node.type() == IHtmlNode::NodeTypeElement &&
			(node.tagId() == IHtmlNode::TagIdFrame || node.tagId() == IHtmlNode::TagIdFrameSet);
	});

	// uncomment to parse iframes
	/*for (const IHtmlNodeCountedPtr& frame: frames)
	{
		if (frame->tagId() != IHtmlNode::TagIdFrame)
		{
			continue;
		}

		Url frameUrl(frame->attribute("src"));

		LinkInfo link{ std::move(frameUrl), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceTagIframe };

		ResourceOnPage resource(ResourceType::ResourceHtml, std::move(link));

		if (page->allResourcesOnPage.find(resource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(resource));
		}
	}*/

	page->hasFrames = !frames.empty();
}

}