#include "serializer.h"
#include "json_parser_stream_writer.h"
#include "isequenced_storage.h"

namespace CrawlerEngine
{

namespace
{
	const QString pagesKey = QLatin1String("pages");

	const QString serializerVersionKey = QLatin1String("serializer_version");
	const QString serializerVersion = QLatin1String("1");
}

class ParsedPageSerializer
{
public:
	ParsedPageSerializer(const ParsedPage* page, int index, std::map<const ParsedPage*, int>& pagesByIndex)
		: m_page(page)
		, m_index(index)
		, m_pagesByIndex(pagesByIndex)
	{
		m_pagesByIndex[page] = index;
	}
	QVariantMap toJson()
	{
		QVariantMap pageMap;
		pageMap[QString("url")] = m_page->url.toDisplayString();
		pageMap[QString("redirectedUrl")] = m_page->redirectedUrl.toDisplayString();
		pageMap[QString("canonicalUrl")] = m_page->canonicalUrl.toDisplayString();
		pageMap[QString("title")] = m_page->title;
		pageMap[QString("contentType")] = m_page->contentType;
		pageMap[QString("metaRefresh")] = m_page->metaRefresh;
		pageMap[QString("metaDescription")] = m_page->metaDescription;
		pageMap[QString("metaKeywords")] = m_page->metaKeywords;
		pageMap[QString("serverResponse")] = m_page->serverResponse;
		pageMap[QString("firstH1")] = m_page->firstH1;
		pageMap[QString("secondH1")] = m_page->secondH1;
		pageMap[QString("firstH2")] = m_page->firstH2;
		pageMap[QString("statusCode")] = static_cast<int>(m_page->statusCode);
		
		QVariantMap metaRobots;
		for (const std::pair<const UserAgentType, MetaRobotsFlags>& flags : m_page->metaRobotsFlags)
		{
			metaRobots[QString::number(static_cast<int>(flags.first))] = static_cast<int>(flags.second);
		}
		pageMap[QString("metaRobotsFlags")] = metaRobots;
		pageMap[QString("responseDate")] = static_cast<unsigned long long>(m_page->responseDate.toSecsSinceEpoch());
		pageMap[QString("lastModifiedDate")] = static_cast<unsigned long long>(m_page->lastModifiedDate.toSecsSinceEpoch());
		pageMap[QString("pageSizeKilobytes")] = m_page->pageSizeKilobytes;
		pageMap[QString("wordCount")] = m_page->wordCount;
		pageMap[QString("pageHash")] = m_page->pageHash;
		pageMap[QString("hasSeveralTitleTags")] = m_page->hasSeveralTitleTags;
		pageMap[QString("hasSeveralMetaDescriptionTags")] = m_page->hasSeveralMetaDescriptionTags;
		pageMap[QString("hasSeveralMetaKeywordsTags")] = m_page->hasSeveralMetaKeywordsTags;
		pageMap[QString("hasSeveralH1Tags")] = m_page->hasSeveralH1Tags;
		pageMap[QString("hasSeveralEqualH2Tags")] = m_page->hasSeveralEqualH2Tags;
		pageMap[QString("isThisExternalPage")] = m_page->isThisExternalPage;
		pageMap[QString("resourceType")] = static_cast<int>(m_page->resourceType);

		QVariantList linksOnThisPage;
		for (const ResourceLink& link : m_page->linksOnThisPage)
		{
			if (link.resource.expired())
			{
				continue;
			}

			const ParsedPage* linkPage = link.resource.lock().get();
			auto it = m_pagesByIndex.find(linkPage);
			ASSERT(it != m_pagesByIndex.end());

			QVariantMap linkValue;
			linkValue["resourceIndex"] = it->second;
			linkValue["linkParameter"] = static_cast<int>(link.linkParameter);
			linkValue["resourceSource"] = static_cast<int>(link.resourceSource);
			linkValue["altOrTitle"] = link.altOrTitle;
			linksOnThisPage.append(linkValue);
		}
		pageMap[QString("linksOnThisPage")] = linksOnThisPage;


		QVariantList linksToThisPage;
		for (const ResourceLink& link : m_page->linksToThisPage)
		{
			if (link.resource.expired())
			{
				continue;
			}

			const ParsedPage* linkPage = link.resource.lock().get();
			auto it = m_pagesByIndex.find(linkPage);
			ASSERT(it != m_pagesByIndex.end());

			QVariantMap linkValue;
			linkValue["resourceIndex"] = it->second;
			linkValue["linkParameter"] = static_cast<int>(link.linkParameter);
			linkValue["resourceSource"] = static_cast<int>(link.resourceSource);
			linkValue["altOrTitle"] = link.altOrTitle;
			linksToThisPage.append(linkValue);
		}
		pageMap[QString("linksToThisPage")] = linksToThisPage;


		pageMap[QString("rawResponse")] = m_page->rawResponse.toBase64();
		pageMap[QString("pageLevel")] = m_page->pageLevel;

		return pageMap;
	}

	ParsedPage* fromJson(const QVariantMap& value)
	{
		Q_UNUSED(value);
		return nullptr;
	}

private:
	const ParsedPage* m_page;
	int m_index;
	std::map<const ParsedPage*, int>& m_pagesByIndex;
};

Serializer::Serializer(const ISequencedStorage* crawledPages)
	: m_crawledPages(crawledPages)
{
}

void Serializer::saveToJsonStream(Common::JsonParserStreamWriter& stream) const
{
	Common::JsonStreamMapElement map(stream);

	map.writeMapValue(serializerVersionKey, serializerVersion);

	Common::JsonStreamMapValue pagesNode(map, pagesKey);
	savePagesToJsonStream(pagesNode);
}

void Serializer::savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const
{
	Q_UNUSED(stream);
	// TODO: implement
	Common::JsonStreamListElement pagesElement(stream);
	std::vector<ParsedPageSerializer> pageWrappers;
	pageWrappers.reserve(m_crawledPages->size());
	std::map<const ParsedPage*, int> pagesByIndex;
	for (int i = 0; i < m_crawledPages->size(); ++i)
	{
		const ParsedPage* page = (*m_crawledPages)[i];
		pageWrappers.emplace_back(page, i, pagesByIndex);
	}

	for (int i = 0; i < m_crawledPages->size(); ++i)
	{
		ParsedPageSerializer& wrapper = pageWrappers[i];
		pagesElement.writeCompound(wrapper.toJson());
	}

	//pagesElement.writeCompound(wrapper.toJson());
}
	
}



