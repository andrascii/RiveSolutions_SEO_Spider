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

// 	auto predicate = [](const GumboNode* node)
// 	{
// 		return node &&
// 			node->type == GUMBO_NODE_ELEMENT &&
// 			node->v.element.tag == GUMBO_TAG_IMG &&
// 			gumbo_get_attribute(&node->v.element.attributes, "src");
// 	};
// 
// 	auto resultGetter = [](const GumboNode* node)
// 	{
// 		const QRegularExpression regExp("[\\n\\t]+");
// 
// 		const GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
// 		const GumboAttribute* alt = gumbo_get_attribute(&node->v.element.attributes, "alt");
// 
// 		const QString altVal = alt == nullptr ? QString() : alt->value;
// 		const Url url = QString(src->value).remove(regExp);
// 
// 		return std::make_pair(url, altVal);
// 	};
// 
// 	std::vector<std::pair<Url, QString>> imageTagHandles = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);
// 
// 	for (std::pair<Url, QString>& url : imageTagHandles)
// 	{
// 		url.first = PageParserHelpers::resolveUrl(page->baseUrl, url.first);
// 
// 		const bool dataResource = url.first.toDisplayString().startsWith(QString("data:"));
// 
// 		const ResourceOnPage imageResource
// 		{
// 			ResourceType::ResourceImage,
// 			LinkInfo{ url.first, LinkParameter::DofollowParameter, url.second, dataResource, ResourceSource::SourceTagImg }
// 		};
// 
// 		if (page->allResourcesOnPage.find(imageResource) == page->allResourcesOnPage.end())
// 		{
// 			page->allResourcesOnPage.insert(std::move(imageResource));
// 		}
// 	}
// 
// 	CompoundParser::parse(output, headers, page);

	//////////////////////////////////////////////////////////////////////////

	struct ImageAttributeHandle
	{
		Url sourceAttributeUrl;
		QString altAttributeValue;
	};

	std::vector<IHtmlNodeSharedPtr> imgTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdImg);

	const auto isBadImgTag = [](const IHtmlNodeSharedPtr& imgTag)
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

	for (const IHtmlNodeSharedPtr& imgTag : imgTags)
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
