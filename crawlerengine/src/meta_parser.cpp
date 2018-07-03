#include "meta_parser.h"
#include "page_parser_helpers.h"
#include "ihtml_parser.h"
#include "meta_robots_helpers.h"

namespace
{

const std::map<QString, CrawlerEngine::MetaRobotsItem> s_metaRobotsMapping
{
	{ QString("all"), CrawlerEngine::MetaRobotsAll },
	{ QString("noindex"), CrawlerEngine::MetaRobotsNoIndex },
	{ QString("nofollow"), CrawlerEngine::MetaRobotsNoFollow },
	{ QString("none"), CrawlerEngine::MetaRobotsNone },
	{ QString("noarchive"),CrawlerEngine::MetaRobotsNoArchive },
	{ QString("nosnippet"), CrawlerEngine::MetaRobotsNoSnippet },
	{ QString("noodp"), CrawlerEngine::MetaRobotsNoODP },
	{ QString("notranslate"), CrawlerEngine::MetaRobotsNoTranslate },
	{ QString("noimageindex"), CrawlerEngine::MetaRobotsNoImageIndex },
	{ QString("index"), CrawlerEngine::MetaRobotsIndex },
	{ QString("follow"), CrawlerEngine::MetaRobotsFollow },
	{ QString("noyaca"), CrawlerEngine::MetaRobotsNoYaCa },
	{ QString("noydir"), CrawlerEngine::MetaRobotsNoYDir }
};

}

namespace CrawlerEngine
{

MetaParser::MetaParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_metaRefreshContentPattern(QRegularExpression("[0-9]+;[[:space:]]*url[[:space:]]*=[[:space:]]*([a-z0-9.\\-:/]+)"))
	, m_eraseCharsRegExp("[\\n\\t]+")
{
}

void MetaParser::parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage)
{
	if (parsedPage->resourceType != ResourceType::ResourceHtml)
	{
		parseMetaRobots(headers, parsedPage);
		return;
	}

	parseMetaContentType(parsedPage);
	parseMetaRefresh(parsedPage);
	parseMetaDescription(parsedPage);
	parseMetaKeywords(parsedPage);
	parseMetaRobots(headers, parsedPage);
}

void MetaParser::parseMetaContentType(ParsedPagePtr& parsedPage) noexcept
{
	std::vector<IHtmlNodeCountedPtr> metaTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdMeta);

	const auto isBadMetaTag = [](const IHtmlNodeCountedPtr& metaTag)
	{
		const bool hasValidHttpEquivAttributeValue = metaTag->hasAttribute("http-equiv") &&
			metaTag->attribute("http-equiv").toLower().trimmed() == "content-type";

		if (!hasValidHttpEquivAttributeValue)
		{
			return true;
		}

		return !metaTag->hasAttribute("content");
	};

	metaTags.erase(std::remove_if(metaTags.begin(), metaTags.end(), isBadMetaTag), metaTags.end());

	std::vector<QString> contentAttributeValues;
	contentAttributeValues.reserve(metaTags.size());

	for (const IHtmlNodeCountedPtr& metaTag : metaTags)
	{
		contentAttributeValues.emplace_back(metaTag->attribute("content"));
	}

	if (!contentAttributeValues.empty())
	{
		parsedPage->contentType = contentAttributeValues.front();
	}
}

void MetaParser::parseMetaRefresh(ParsedPagePtr& parsedPage) noexcept
{
	std::vector<IHtmlNodeCountedPtr> metaTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdMeta);

	const auto isBadMetaTag = [](const IHtmlNodeCountedPtr& metaTag)
	{
		const bool hasValidHttpEquivAttributeValue = metaTag->hasAttribute("http-equiv") &&
			metaTag->attribute("http-equiv").toLower().trimmed() == "refresh";

		return !hasValidHttpEquivAttributeValue;
	};

	metaTags.erase(std::remove_if(metaTags.begin(), metaTags.end(), isBadMetaTag), metaTags.end());

	std::vector<QString> contentAttributeValues;
	contentAttributeValues.reserve(metaTags.size());

	for (const IHtmlNodeCountedPtr& metaTag : metaTags)
	{
		contentAttributeValues.emplace_back(metaTag->attribute("content"));
	}

	if (!contentAttributeValues.empty())
	{
		parsedPage->metaRefresh = contentAttributeValues.back();
		parsedPage->hasMetaRefreshTag = true;
	}

	foreach(const QString& content, contentAttributeValues)
	{
		const QRegularExpressionMatch match = m_metaRefreshContentPattern.match(content);
		const QStringList matchParts = match.capturedTexts();

		if (matchParts.size() < 2)
		{
			continue;
		}

		Url contentUrl(matchParts[1]);

		if (contentUrl == parsedPage->url)
		{
			continue;
		}

		LinkInfo link{ std::move(contentUrl), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceTagMetaRefresh };

		ResourceOnPage resource(ResourceType::ResourceHtml, std::move(link));

		if (parsedPage->allResourcesOnPage.find(resource) == parsedPage->allResourcesOnPage.end())
		{
			parsedPage->allResourcesOnPage.insert(std::move(resource));
		}
	}
}

void MetaParser::parseMetaDescription(ParsedPagePtr& parsedPage) noexcept
{
	std::vector<IHtmlNodeCountedPtr> metaTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdMeta);

	const auto isBadMetaTag = [](const IHtmlNodeCountedPtr& metaTag)
	{
		const bool hasValidAttributeValue = metaTag->hasAttribute("name") &&
			metaTag->attribute("name").toLower().trimmed() == "description";

		if (!hasValidAttributeValue)
		{
			return true;
		}

		return !metaTag->hasAttribute("content");
	};

	metaTags.erase(std::remove_if(metaTags.begin(), metaTags.end(), isBadMetaTag), metaTags.end());

	std::vector<QString> contentAttributeValues;
	contentAttributeValues.reserve(metaTags.size());

	for (const IHtmlNodeCountedPtr& metaTag : metaTags)
	{
		contentAttributeValues.emplace_back(QString(metaTag->attribute("content")).remove(m_eraseCharsRegExp));
	}

	if (!contentAttributeValues.empty())
	{
		parsedPage->metaDescription = contentAttributeValues.front();
	}

	parsedPage->hasSeveralMetaDescriptionTags = contentAttributeValues.size() > 1;
}

void MetaParser::parseMetaKeywords(ParsedPagePtr& parsedPage) noexcept
{
	std::vector<IHtmlNodeCountedPtr> metaTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdMeta);

	const auto isBadMetaTag = [](const IHtmlNodeCountedPtr& metaTag)
	{
		const bool hasValidAttributeValue = metaTag->hasAttribute("name") &&
			metaTag->attribute("name").toLower().trimmed() == "keywords";

		if (!hasValidAttributeValue)
		{
			return true;
		}

		return !metaTag->hasAttribute("content");
	};

	metaTags.erase(std::remove_if(metaTags.begin(), metaTags.end(), isBadMetaTag), metaTags.end());

	std::vector<QString> contentAttributeValues;
	contentAttributeValues.reserve(metaTags.size());

	for (const IHtmlNodeCountedPtr& metaTag : metaTags)
	{
		contentAttributeValues.emplace_back(QString(metaTag->attribute("content")).remove(m_eraseCharsRegExp));
	}

	if (!contentAttributeValues.empty())
	{
		parsedPage->metaKeywords = contentAttributeValues.front();
	}

	parsedPage->hasSeveralMetaKeywordsTags = contentAttributeValues.size() > 1;
}

void MetaParser::parseMetaRobots(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) noexcept
{
	std::vector<QString> xRobotsTagValues = headers.valueOf("x-robots-tag");

	for (size_t i = 0; i < xRobotsTagValues.size(); ++i)
	{
		const QStringList parts = xRobotsTagValues[i].split(QLatin1Char(','), QString::SkipEmptyParts);

		for (const QString& part : parts)
		{
			auto it = s_metaRobotsMapping.find(part.trimmed().toLower());

			if (it != s_metaRobotsMapping.cend())
			{
				parsedPage->metaRobotsFlags[UserAgentType::AnyBot].setFlag(it->second);
			}
		}
	}

	if (parsedPage->resourceType != ResourceType::ResourceHtml)
	{
		// non-html, process only headers
		return;
	}

	const QStringList supportedUserAgents = MetaRobotsHelpers::supportedUserAgents(true);

	foreach(const QString& userAgentStr, supportedUserAgents)
	{
		std::vector<IHtmlNodeCountedPtr> metaTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdMeta);

		const auto isBadMetaTag = [&userAgentStr](const IHtmlNodeCountedPtr& metaTag)
		{
			const bool hasValidAttributeValue = metaTag->hasAttribute("name") &&
				metaTag->attribute("name").toLower().trimmed() == userAgentStr;

			if (!hasValidAttributeValue)
			{
				return true;
			}

			return !metaTag->hasAttribute("content");
		};

		metaTags.erase(std::remove_if(metaTags.begin(), metaTags.end(), isBadMetaTag), metaTags.end());

		std::vector<QString> contentAttributeValues;
		contentAttributeValues.reserve(metaTags.size());

		for (const IHtmlNodeCountedPtr& metaTag : metaTags)
		{
			contentAttributeValues.emplace_back(QString(metaTag->attribute("content")).trimmed().remove(m_eraseCharsRegExp));
		}

		const UserAgentType userAgentType = MetaRobotsHelpers::userAgent(userAgentStr);

		ASSERT(userAgentType != UserAgentType::Unknown);

		for (const QString& robotsItem : contentAttributeValues)
		{
			const QStringList parts = robotsItem.split(QLatin1Char(','), QString::SkipEmptyParts);

			for (const QString& part : parts)
			{
				const auto it = s_metaRobotsMapping.find(part.trimmed().toLower());

				if (it != s_metaRobotsMapping.cend())
				{
					parsedPage->metaRobotsFlags[userAgentType].setFlag(it->second);
				}
			}
		}
	}
}

}
