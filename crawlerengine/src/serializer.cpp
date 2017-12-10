#include "serializer.h"
#include "json_parser_stream_writer.h"
#include "isequenced_storage.h"
#include "json_parser_stream_reader.h"
#include "unique_link_store.h"

namespace CrawlerEngine
{

namespace
{
	const QString pagesCountKey = QLatin1String("pagesCount");
	const QString pagesKey = QLatin1String("pages");

	const QString serializerVersionKey = QLatin1String("serializer_version");
	const QString serializerVersion = QLatin1String("1");

	const QString urlKey = QLatin1String("url");
	const QString redirectedUrlKey = QLatin1String("redirectedUrl");
	const QString canonicalUrlKey = QLatin1String("canonicalUrl");
	const QString titleKey = QLatin1String("title");
	const QString contentTypeKey = QLatin1String("contentType");
	const QString metaRefreshKey = QLatin1String("metaRefresh");
	const QString metaDescriptionKey = QLatin1String("metaDescription");
	const QString metaKeywordsKey = QLatin1String("metaKeywords");
	const QString serverResponseKey = QLatin1String("serverResponse");
	const QString firstH1Key = QLatin1String("firstH1");
	const QString secondH1Key = QLatin1String("secondH1");
	const QString firstH2Key = QLatin1String("firstH2");
	const QString statusCodeKey = QLatin1String("statusCode");
	const QString metaRobotsFlagsKey = QLatin1String("metaRobotsFlags");
	const QString responseDateKey = QLatin1String("responseDate");
	const QString lastModifiedDateKey = QLatin1String("lastModifiedDate");
	const QString pageSizeKilobytesKey = QLatin1String("pageSizeKilobytes");
	const QString wordCountKey = QLatin1String("wordCount");
	const QString pageHashKey = QLatin1String("pageHash");
	const QString hasSeveralTitleTagsKey = QLatin1String("hasSeveralTitleTags");
	const QString hasSeveralMetaDescriptionTagsKey = QLatin1String("hasSeveralMetaDescriptionTags");
	const QString hasSeveralMetaKeywordsTagsKey = QLatin1String("hasSeveralMetaKeywordsTags");
	const QString hasSeveralH1TagsKey = QLatin1String("hasSeveralH1Tags");
	const QString hasSeveralEqualH2TagsKey = QLatin1String("hasSeveralEqualH2Tags");
	const QString isThisExternalPageKey = QLatin1String("isThisExternalPage");
	const QString resourceTypeKey = QLatin1String("resourceType");
	const QString rawResponseKey = QLatin1String("rawResponse");
	const QString pageLevelKey = QLatin1String("pageLevel");
	const QString linksOnThisPageKey = QLatin1String("linksOnThisPage");
	const QString linkParameterKey = QLatin1String("linkParameter");
	const QString resourceSourceKey = QLatin1String("resourceSource");
	const QString altOrTitleKey = QLatin1String("altOrTitle");
	const QString resourceIndexKey = QLatin1String("resourceIndex");
	const QString requestTypeKey = QLatin1String("requestType");
	const QString crawledUrlsKey = QLatin1String("crawledUrls");
	const QString pendingUrlsKey = QLatin1String("pendingUrls");
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
		pageMap[urlKey] = m_page->url.toDisplayString();
		pageMap[redirectedUrlKey] = m_page->redirectedUrl.toDisplayString();
		pageMap[canonicalUrlKey] = m_page->canonicalUrl.toDisplayString();
		pageMap[titleKey] = m_page->title;
		pageMap[contentTypeKey] = m_page->contentType;
		pageMap[metaRefreshKey] = m_page->metaRefresh;
		pageMap[metaDescriptionKey] = m_page->metaDescription;
		pageMap[metaKeywordsKey] = m_page->metaKeywords;
		pageMap[serverResponseKey] = m_page->serverResponse;
		pageMap[firstH1Key] = m_page->firstH1;
		pageMap[secondH1Key] = m_page->secondH1;
		pageMap[firstH2Key] = m_page->firstH2;
		pageMap[statusCodeKey] = static_cast<int>(m_page->statusCode);
		
		QVariantMap metaRobots;
		for (const std::pair<const UserAgentType, MetaRobotsFlags>& flags : m_page->metaRobotsFlags)
		{
			metaRobots[QString::number(static_cast<int>(flags.first))] = static_cast<int>(flags.second);
		}
		pageMap[metaRobotsFlagsKey] = metaRobots;
		pageMap[responseDateKey] = static_cast<unsigned long long>(m_page->responseDate.toMSecsSinceEpoch());
		pageMap[lastModifiedDateKey] = static_cast<unsigned long long>(m_page->lastModifiedDate.toMSecsSinceEpoch());
		pageMap[pageSizeKilobytesKey] = m_page->pageSizeKilobytes;
		pageMap[wordCountKey] = m_page->wordCount;
		pageMap[pageHashKey] = m_page->pageHash;
		pageMap[hasSeveralTitleTagsKey] = m_page->hasSeveralTitleTags;
		pageMap[hasSeveralMetaDescriptionTagsKey] = m_page->hasSeveralMetaDescriptionTags;
		pageMap[hasSeveralMetaKeywordsTagsKey] = m_page->hasSeveralMetaKeywordsTags;
		pageMap[hasSeveralH1TagsKey] = m_page->hasSeveralH1Tags;
		pageMap[hasSeveralEqualH2TagsKey] = m_page->hasSeveralEqualH2Tags;
		pageMap[isThisExternalPageKey] = m_page->isThisExternalPage;
		pageMap[resourceTypeKey] = static_cast<int>(m_page->resourceType);

		QVariantList linksOnThisPage;
		for (const ResourceLink& link : m_page->linksOnThisPage)
		{
			if (link.resource.expired())
			{
				continue;
			}

			const ParsedPage* linkPage = link.resource.lock().get();
			auto it = m_pagesByIndex.find(linkPage);

			// seems like the page can be pending and can be no found in m_pagesByIndex
			// probably we should somehow store the pending pages too
			// TODO: implement what is described above
			QVariantMap linkValue;
			linkValue[resourceIndexKey] = it != m_pagesByIndex.end() ? it->second : -1;
			linkValue[urlKey] = link.url.toDisplayString();
			linkValue[linkParameterKey] = static_cast<int>(link.linkParameter);
			linkValue[resourceSourceKey] = static_cast<int>(link.resourceSource);
			linkValue[altOrTitleKey] = link.altOrTitle;
			linksOnThisPage.append(linkValue);
		}
		pageMap[linksOnThisPageKey] = linksOnThisPage;

		pageMap[rawResponseKey] = m_page->rawResponse.toBase64();
		pageMap[pageLevelKey] = m_page->pageLevel;

		return pageMap;
	}

private:
	const ParsedPage* m_page;
	int m_index;
	std::map<const ParsedPage*, int>& m_pagesByIndex;
};

class ParsedPageDeserializer
{
public:
	ParsedPageDeserializer(int index, std::map<int, ParsedPage*>& pagesByIndex)
		: m_page(new ParsedPage())
		, m_index(index)
		, m_pagesByIndex(pagesByIndex)
	{
		m_pagesByIndex[index] = m_page;
	}

	void resolveLinks()
	{
		foreach(const QVariant& link, m_linksOnThisPage)
		{
			QVariantMap linkMap = link.toMap();
			const LinkParameter linkParameter = static_cast<LinkParameter>(linkMap[linkParameterKey].toInt());
			const ResourceSource resourceSource = static_cast<ResourceSource>(linkMap[resourceSourceKey].toInt());
			const QString altOrTitle = linkMap[altOrTitleKey].toString();
			const QUrl url = QUrl(linkMap[urlKey].toString());

			const int resourceIndex = linkMap[resourceIndexKey].toInt();

			ResourceType resourceType = ResourceType::ResourceHtml;
			if (resourceIndex != -1)
			{
				auto it = m_pagesByIndex.find(resourceIndex);
				ASSERT(it != m_pagesByIndex.end());
				resourceType = it->second->resourceType;
			}

			const RawResourceOnPage resource
			{
				resourceType,
				LinkInfo{ url, linkParameter, altOrTitle, false, resourceSource }
			};

			m_page->allResourcesOnPage.push_back(resource);
		}
	}

	void fromJson(const QVariantMap& pageMap)
	{
		m_page->url = QUrl(pageMap[urlKey].toString());
		m_page->redirectedUrl = QUrl(pageMap[redirectedUrlKey].toString());
		m_page->canonicalUrl = QUrl(pageMap[canonicalUrlKey].toString());
		m_page->title = pageMap[titleKey].toString();
		m_page->contentType = pageMap[contentTypeKey].toString();
		m_page->metaRefresh = pageMap[metaRefreshKey].toString();
		m_page->metaDescription = pageMap[metaDescriptionKey].toString();
		m_page->metaKeywords = pageMap[metaKeywordsKey].toString();
		m_page->serverResponse = pageMap[serverResponseKey].toString();
		m_page->firstH1 = pageMap[firstH1Key].toString();
		m_page->secondH1 = pageMap[secondH1Key].toString();
		m_page->firstH2 = pageMap[firstH2Key].toString();
		m_page->statusCode = static_cast<Common::StatusCode>(pageMap[statusCodeKey].toInt());

		QVariantMap metaRobots = pageMap[metaRobotsFlagsKey].toMap();
		foreach(const QString& userAgentStr, metaRobots.keys())
		{
			const UserAgentType userAgentType = static_cast<UserAgentType>(userAgentStr.toInt());
			const MetaRobotsFlags flags = MetaRobotsFlags(metaRobots[userAgentStr].toInt());
			m_page->metaRobotsFlags[userAgentType] = flags;
		}

		m_page->responseDate = QDateTime::fromMSecsSinceEpoch(qvariant_cast<unsigned long long>(pageMap[responseDateKey]));
		m_page->lastModifiedDate = QDateTime::fromMSecsSinceEpoch(qvariant_cast<unsigned long long>(pageMap[lastModifiedDateKey]));
		m_page->pageSizeKilobytes = pageMap[pageSizeKilobytesKey].toInt();
		m_page->wordCount = pageMap[wordCountKey].toInt();
		m_page->pageHash = qvariant_cast<size_t>(pageMap[pageHashKey]);
		m_page->hasSeveralTitleTags = pageMap[hasSeveralTitleTagsKey].toBool();
		m_page->hasSeveralMetaDescriptionTags = pageMap[hasSeveralMetaDescriptionTagsKey].toBool();
		m_page->hasSeveralMetaKeywordsTags = pageMap[hasSeveralMetaKeywordsTagsKey].toBool();
		m_page->hasSeveralH1Tags = pageMap[hasSeveralH1TagsKey].toBool();
		m_page->hasSeveralEqualH2Tags = pageMap[hasSeveralEqualH2TagsKey].toBool();
		m_page->isThisExternalPage = pageMap[isThisExternalPageKey].toBool();
		m_page->resourceType = static_cast<ResourceType>(pageMap[resourceTypeKey].toInt());
		m_page->rawResponse = QByteArray::fromBase64(pageMap[rawResponseKey].toByteArray());
		m_page->pageLevel = pageMap[pageLevelKey].toInt();

		m_linksOnThisPage = pageMap[linksOnThisPageKey].toList();
	}

	ParsedPage* page() const
	{
		return m_page;
	}

private:
	ParsedPage* m_page;
	int m_index;
	std::map<int, ParsedPage*>& m_pagesByIndex;
	QVariantList m_linksOnThisPage;
};

Serializer::Serializer()
{
}

Serializer::Serializer(std::vector<ParsedPage*>&& pages, std::vector<CrawlerRequest>&& crawledUrls, std::vector<CrawlerRequest>&& pendingUrls)
	: m_pages(std::move(pages))
	, m_crawledLinks(std::move(crawledUrls))
	, m_pendingLinks(std::move(pendingUrls))
{
}

void Serializer::saveToJsonStream(Common::JsonParserStreamWriter& stream)
{
	Common::JsonStreamMapElement map(stream);

	map.writeMapValue(serializerVersionKey, serializerVersion);
	map.writeMapValue(pagesCountKey, m_pages.size());

	{
		Common::JsonStreamMapValue pagesNode(map, pagesKey);
		savePagesToJsonStream(pagesNode);
	}
	
	{
		Common::JsonStreamMapValue crawledLinksNode(map, crawledUrlsKey);
		saveLinksToJsonStream(crawledLinksNode, m_crawledLinks);
	}
	
	{
		Common::JsonStreamMapValue pendingLinksNode(map, pendingUrlsKey);
		saveLinksToJsonStream(pendingLinksNode, m_pendingLinks);
	}
}

void Serializer::readFromJsonStream(Common::JsonParserStreamReader& stream)
{
	Common::JsonStreamMapReader map(stream);
	const int pagesCount = map.readValue(pagesCountKey).toInt();
	readPagesFromJsonStream(map.value(pagesKey), pagesCount);

	readLinksFromJsonStream(map.value(crawledUrlsKey), m_crawledLinks);
	readLinksFromJsonStream(map.value(pendingUrlsKey), m_pendingLinks);
}

const std::vector<ParsedPage*>& CrawlerEngine::Serializer::pages() const
{
	return m_pages;
}

const std::vector<CrawlerRequest>& Serializer::crawledLinks() const
{
	return m_crawledLinks;
}

const std::vector<CrawlerRequest>& CrawlerEngine::Serializer::pendingLinks() const
{
	return m_pendingLinks;
}

void Serializer::savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const
{
	Common::JsonStreamListElement pagesElement(stream);
	std::vector<ParsedPageSerializer> pageWrappers;
	pageWrappers.reserve(m_pages.size());
	std::map<const ParsedPage*, int> pagesByIndex;
	for (int i = 0; i < m_pages.size(); ++i)
	{
		const ParsedPage* page = m_pages[i];
		pageWrappers.emplace_back(page, i, pagesByIndex);
	}

	for (int i = 0; i < m_pages.size(); ++i)
	{
		ParsedPageSerializer& wrapper = pageWrappers[i];
		pagesElement.writeCompound(wrapper.toJson());
	}
}

void Serializer::readPagesFromJsonStream(Common::JsonParserStreamReader& stream, int pagesCount)
{
	Common::JsonStreamListReader reader(stream);
	
	std::vector<ParsedPageDeserializer> pageWrappers;
	pageWrappers.reserve(pagesCount);
	std::map<int, ParsedPage*> pagesByIndex;

	int index = 0;
	while (!reader.endOfList())
	{
		QVariantMap pageMap = reader.readCompound().toMap();
		ParsedPageDeserializer& wrapper = pageWrappers.emplace_back(index, pagesByIndex);
		wrapper.fromJson(pageMap);
		++index;
	}

	for (ParsedPageDeserializer& wrapper: pageWrappers)
	{
		wrapper.resolveLinks();
		m_pages.push_back(wrapper.page());
	}
}
void Serializer::saveLinksToJsonStream(Common::JsonParserStreamWriter& stream, const std::vector<CrawlerRequest>& links) const
{
	Common::JsonStreamListElement linksListElement(stream);
	for (const CrawlerRequest& link : links)
	{
		QVariantMap linkMap;
		linkMap[urlKey] = link.url.toDisplayString();
		linkMap[requestTypeKey] = static_cast<int>(link.requestType);

		linksListElement.writeCompound(linkMap);
	}
}
void CrawlerEngine::Serializer::readLinksFromJsonStream(Common::JsonParserStreamReader& stream, std::vector<CrawlerRequest>& links)
{
	Common::JsonStreamListReader reader(stream);

	while (!reader.endOfList())
	{
		QVariantMap linkMap = reader.readCompound().toMap();
		CrawlerRequest request;
		request.url = QUrl(linkMap[urlKey].toString());
		request.requestType = static_cast<DownloadRequestType>(linkMap[requestTypeKey].toInt());
		links.push_back(request);
	}
}
}



