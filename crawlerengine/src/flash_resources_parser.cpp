#include "flash_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "ihtml_parser.h"

namespace
{

const QByteArray s_typeEmbedTagAttributeValueToken = "application/x-shockwave-flash";
const QByteArray s_classIdAttributeValueToken = "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000";

}

namespace CrawlerEngine
{

FlashResourcesParser::FlashResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceFlash));
}

void FlashResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseFlashResourcesV1(page);
	parseFlashResourcesV2(page);
	parseFlashResourcesV3(page);

	CompoundParser::parse(headers, page);
}

void FlashResourcesParser::parseFlashResourcesV1(ParsedPagePtr& page) noexcept
{
	//<embed type="application/x-shockwave-flash" src="myContent.swf" width="300" height="120" pluginspage="http://www.adobe.com/go/getflashplayer" / >
	//<noembed>Alternative content</noembed>

	DEBUG_ASSERT(page->baseUrl.isValid());

	std::vector<IHtmlNodeSharedPtr> embedTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdEmbed);

	const auto isBadEmbedTag = [](const IHtmlNodeSharedPtr& embedTag)
	{
		if (!embedTag->hasAttribute("src"))
		{
			return true;
		}

		const bool hasValidTypeAttributeValue = embedTag->hasAttribute("type") &&
			embedTag->attribute("type").toLower().trimmed() == s_typeEmbedTagAttributeValueToken;

		return !hasValidTypeAttributeValue;
	};

	embedTags.erase(std::remove_if(embedTags.begin(), embedTags.end(), isBadEmbedTag), embedTags.end());

	std::vector<Url> sourceUrls;
	sourceUrls.reserve(embedTags.size());

	for (const IHtmlNodeSharedPtr& embedTag : embedTags)
	{
		sourceUrls.emplace_back(embedTag->attribute("src").trimmed().remove(m_regExp));
	}

	const std::vector<Url> resolvedSourceUrls = PageParserHelpers::resolveUrlList(page->baseUrl, sourceUrls);

	for (const Url& url : resolvedSourceUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource },
		};

		if (page->allResourcesOnPage.find(flashResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(flashResource));
		}
	}
}

void FlashResourcesParser::parseFlashResourcesV2(ParsedPagePtr& page) noexcept
{
	// 	<object type="application/x-shockwave-flash" data="myContent.swf" width="300" height="120">
	// 	<p>Alternative content</p>
	// 	</object>

	DEBUG_ASSERT(page->baseUrl.isValid());

	std::vector<IHtmlNodeSharedPtr> embedTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdEmbed);

	const auto isBadEmbedTag = [](const IHtmlNodeSharedPtr& embedTag)
	{
		if (!embedTag->hasAttribute("data"))
		{
			return true;
		}

		const bool hasValidTypeAttributeValue = embedTag->hasAttribute("type") &&
			embedTag->attribute("type").toLower().trimmed() == s_typeEmbedTagAttributeValueToken;

		return !hasValidTypeAttributeValue;
	};

	embedTags.erase(std::remove_if(embedTags.begin(), embedTags.end(), isBadEmbedTag), embedTags.end());

	std::vector<Url> dataAttributeUrls;
	dataAttributeUrls.reserve(embedTags.size());

	for (const IHtmlNodeSharedPtr& embedTag : embedTags)
	{
		dataAttributeUrls.emplace_back(embedTag->attribute("data").trimmed().remove(m_regExp));
	}

	const std::vector<Url> resolvedDataAttributeUrls = PageParserHelpers::resolveUrlList(page->baseUrl, dataAttributeUrls);

	for (const Url& url : resolvedDataAttributeUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		if (page->allResourcesOnPage.find(flashResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(flashResource));
		}
	}
}

void FlashResourcesParser::parseFlashResourcesV3(ParsedPagePtr& page) noexcept
{
	// 	<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="300" height="120">
	// 	<param name="movie" value="myContent.swf" />
	// 	<p>Alternative content</p>
	// 	</object>

	DEBUG_ASSERT(page->baseUrl.isValid());

	std::vector<IHtmlNodeSharedPtr> objectTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdObject);

	const auto isBadObjectTag = [](const IHtmlNodeSharedPtr& objectTag)
	{
		const bool hasValidClassIdAttributeValue = objectTag->hasAttribute("classid") &&
			objectTag->attribute("classid").toLower().trimmed() == s_classIdAttributeValueToken;

		if (!hasValidClassIdAttributeValue)
		{
			return true;
		}

		const bool hasParamChildTag = objectTag->childNodeByAttributeValue(IHtmlNode::TagIdParam, std::make_pair("movie", "")) ||
			objectTag->childNodeByAttributeValue(IHtmlNode::TagIdParam, std::make_pair("src", ""));

		return !hasParamChildTag;
	};

	objectTags.erase(std::remove_if(objectTags.begin(), objectTags.end(), isBadObjectTag), objectTags.end());

	std::vector<Url> flashResourceUrls;
	flashResourceUrls.reserve(objectTags.size());

	for (const IHtmlNodeSharedPtr& objectTag : objectTags)
	{
		IHtmlNodeSharedPtr paramChildNode = objectTag->childNodeByAttributeValue(IHtmlNode::TagIdParam, std::make_pair("movie", ""));

		if (paramChildNode)
		{
			flashResourceUrls.emplace_back(paramChildNode->attribute("movie").trimmed().remove(m_regExp));
			continue;
		}

		paramChildNode = objectTag->childNodeByAttributeValue(IHtmlNode::TagIdParam, std::make_pair("src", ""));

		DEBUG_ASSERT(paramChildNode);

		flashResourceUrls.emplace_back(paramChildNode->attribute("src").trimmed().remove(m_regExp));
	}

	const std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->baseUrl, flashResourceUrls);

	for (const Url& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		if (page->allResourcesOnPage.find(flashResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(flashResource));
		}
	}
}

}
