#include "video_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

VideoResourcesParser::VideoResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceVideo));
}

void VideoResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	std::vector<IHtmlNodeSharedPtr> videoTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdVideo);

	std::vector<Url> videoResourceUrls;
	videoResourceUrls.reserve(videoTags.size());

	for (const IHtmlNodeSharedPtr& videoTag : videoTags)
	{
		IHtmlNodeSharedPtr paramChildNode = videoTag->childNodeByAttributeValue(IHtmlNode::TagIdSource, std::make_pair("src", ""));

		if (paramChildNode)
		{
			videoResourceUrls.emplace_back(paramChildNode->attribute("src").trimmed().remove(m_regExp));
			continue;
		}

		if (!videoTag->hasAttribute("src"))
		{
			continue;
		}

		videoResourceUrls.emplace_back(videoTag->attribute("src").trimmed().remove(m_regExp));
	}

	const std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, videoResourceUrls);

	for (const Url& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage videoResource
		{
			ResourceType::ResourceVideo,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		if (page->allResourcesOnPage.find(videoResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(videoResource));
		}
	}

	CompoundParser::parse(headers, page);
}

}
