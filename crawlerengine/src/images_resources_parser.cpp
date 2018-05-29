#include "images_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

ImagesResourcesParser::ImagesResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceImage));
}

void ImagesResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	struct ImageAttributeHandle
	{
		Url sourceAttributeUrl;
		QString altAttributeValue;
	};

	std::vector<IHtmlNodeCountedPtr> imgTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdImg);

	const auto isBadImgTag = [](const IHtmlNodeCountedPtr& imgTag)
	{
		if (imgTag->type() != IHtmlNode::NodeTypeElement)
		{
			return true;
		}

		return !imgTag->hasAttribute("src");
	};

	imgTags.erase(std::remove_if(imgTags.begin(), imgTags.end(), isBadImgTag), imgTags.end());

	std::vector<ImageAttributeHandle> imageTagHandles;
	imageTagHandles.reserve(imgTags.size());

	for (const IHtmlNodeCountedPtr& imgTag : imgTags)
	{
		imageTagHandles.push_back(ImageAttributeHandle{ imgTag->attribute("src").remove(m_regExp), imgTag->attribute("alt").remove(m_regExp) });
	}

	for (ImageAttributeHandle& imageTagHandle : imageTagHandles)
	{
		imageTagHandle.sourceAttributeUrl = PageParserHelpers::resolveUrl(page->baseUrl, imageTagHandle.sourceAttributeUrl);

		const bool dataResource = imageTagHandle.sourceAttributeUrl.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage imageResource
		{
			ResourceType::ResourceImage,
			LinkInfo{ imageTagHandle.sourceAttributeUrl, LinkParameter::DofollowParameter, imageTagHandle.altAttributeValue, dataResource, ResourceSource::SourceTagImg }
		};

		if (page->allResourcesOnPage.find(imageResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(imageResource));
		}
	}

	CompoundParser::parse(headers, page);
}

}
