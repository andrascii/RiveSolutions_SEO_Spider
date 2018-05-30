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

	std::vector<IHtmlNodeCountedPtr> videoTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdVideo);

	std::vector<Url> videoResourceUrls;
	videoResourceUrls.reserve(videoTags.size());

	for (const IHtmlNodeCountedPtr& videoTag : videoTags)
	{
		IHtmlNodeCountedPtr paramChildNode = videoTag->childNodeByAttributeValue(IHtmlNode::TagIdSource, std::make_pair("src", ""));

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

	PageParserHelpers::resolveUrlList(page->url, videoResourceUrls);

	for (const Url& url : videoResourceUrls)
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
