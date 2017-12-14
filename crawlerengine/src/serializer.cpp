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
	const QString pageKey = QLatin1String("page");

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
	const QString metaRobotsFlagsItemKey = QLatin1String("metaRobotsItem");
	const QString metaRobotsValueKey = QLatin1String("metaRobotsValue");
	const QString userAgentKey = QLatin1String("userAgent");
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
	const QString linksOnThisPageItemKey = QLatin1String("link");
	const QString linkParameterKey = QLatin1String("linkParameter");
	const QString resourceSourceKey = QLatin1String("resourceSource");
	const QString altOrTitleKey = QLatin1String("altOrTitle");
	const QString resourceIndexKey = QLatin1String("resourceIndex");
	const QString requestTypeKey = QLatin1String("requestType");
	const QString crawledUrlsKey = QLatin1String("crawledUrls");
	const QString pendingUrlsKey = QLatin1String("pendingUrls");
	const QString urlItemKey = QLatin1String("urlItem");
	const QString storagesKey = QLatin1String("storages");
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
			QVariantMap linkValue;
			linkValue[urlKey] = link.url.toDisplayString();
			linkValue[linkParameterKey] = static_cast<int>(link.linkParameter);
			linkValue[resourceSourceKey] = static_cast<int>(link.resourceSource);
			linkValue[altOrTitleKey] = link.altOrTitle;


			if (!link.resource.expired())
			{
				const ParsedPage* linkPage = link.resource.lock().get();
				auto it = m_pagesByIndex.find(linkPage);
				ASSERT(it != m_pagesByIndex.end());

				linkValue[resourceIndexKey] = it->second;
			}
			else
			{
				linkValue[resourceIndexKey] = -1;
			}

			linksOnThisPage.append(linkValue);
		}
		pageMap[linksOnThisPageKey] = linksOnThisPage;

		pageMap[rawResponseKey] = m_page->rawResponse.toBase64();
		pageMap[pageLevelKey] = m_page->pageLevel;

		QString storagesStr;
		for (int i = 0; i < m_page->storages.size(); ++i)
		{
			storagesStr = storagesStr % ( m_page->storages[i] ? QLatin1Char('1') : QLatin1Char('0'));
		}

		pageMap[storagesKey] = storagesStr;

		return pageMap;
	}

	void writeXml(QXmlStreamWriter& writer)
	{
		writer.writeStartElement(pageKey);

		writer.writeTextElement(urlKey, m_page->url.toDisplayString());
		writer.writeTextElement(redirectedUrlKey, m_page->redirectedUrl.toDisplayString());
		writer.writeTextElement(canonicalUrlKey, m_page->canonicalUrl.toDisplayString());
		writer.writeTextElement(titleKey, m_page->title);
		writer.writeTextElement(contentTypeKey, m_page->contentType);
		writer.writeTextElement(metaRefreshKey, m_page->metaRefresh);
		writer.writeTextElement(metaDescriptionKey, m_page->metaDescription);
		writer.writeTextElement(metaKeywordsKey, m_page->metaKeywords);
		writer.writeTextElement(serverResponseKey, m_page->serverResponse);
		writer.writeTextElement(firstH1Key, m_page->firstH1);
		writer.writeTextElement(secondH1Key, m_page->secondH1);
		writer.writeTextElement(firstH2Key, m_page->firstH2);
		writer.writeTextElement(statusCodeKey, QString::number(static_cast<int>(m_page->statusCode)));
	
		{
			writer.writeStartElement(metaRobotsFlagsKey);
			for (const std::pair<const UserAgentType, MetaRobotsFlags>& flags : m_page->metaRobotsFlags)
			{
				writer.writeStartElement(metaRobotsFlagsItemKey);
				writer.writeAttribute(userAgentKey, QString::number(static_cast<int>(flags.first)));
				writer.writeAttribute(metaRobotsValueKey, QString::number(static_cast<int>(flags.second)));
				writer.writeEndElement();
			}

			writer.writeEndElement();
		}
		

		writer.writeTextElement(responseDateKey, QString::number(static_cast<unsigned long long>(m_page->responseDate.toMSecsSinceEpoch())));
		writer.writeTextElement(lastModifiedDateKey, QString::number(static_cast<unsigned long long>(m_page->lastModifiedDate.toMSecsSinceEpoch())));
		writer.writeTextElement(pageSizeKilobytesKey, QString::number(m_page->pageSizeKilobytes));
		writer.writeTextElement(wordCountKey, QString::number(m_page->wordCount));
		writer.writeTextElement(hasSeveralTitleTagsKey, QString::number(m_page->hasSeveralTitleTags));
		writer.writeTextElement(hasSeveralMetaDescriptionTagsKey, QString::number(m_page->hasSeveralMetaDescriptionTags));
		writer.writeTextElement(hasSeveralMetaKeywordsTagsKey, QString::number(m_page->hasSeveralMetaKeywordsTags));
		writer.writeTextElement(hasSeveralH1TagsKey, QString::number(m_page->hasSeveralH1Tags));
		writer.writeTextElement(hasSeveralEqualH2TagsKey, QString::number(m_page->hasSeveralEqualH2Tags));
		writer.writeTextElement(isThisExternalPageKey, QString::number(m_page->isThisExternalPage));
		writer.writeTextElement(resourceTypeKey, QString::number(static_cast<int>(m_page->resourceType)));
		
		{
			writer.writeStartElement(linksOnThisPageKey);

			for (const ResourceLink& link : m_page->linksOnThisPage)
			{
				writer.writeStartElement(linksOnThisPageItemKey);
				
				writer.writeAttribute(urlKey, link.url.toDisplayString());
				writer.writeAttribute(linkParameterKey, QString::number(static_cast<int>(link.linkParameter)));
				writer.writeAttribute(resourceSourceKey, QString::number(static_cast<int>(link.resourceSource)));
				writer.writeAttribute(altOrTitleKey, link.altOrTitle);

				if (!link.resource.expired())
				{
					const ParsedPage* linkPage = link.resource.lock().get();
					auto it = m_pagesByIndex.find(linkPage);
					ASSERT(it != m_pagesByIndex.end());

					writer.writeAttribute(resourceIndexKey, QString::number(it->second));
				}
				else
				{
					writer.writeAttribute(resourceIndexKey, QString::number(-1));
				}

				writer.writeEndElement();
			}

			writer.writeEndElement();
		}

		writer.writeTextElement(rawResponseKey, m_page->rawResponse.toBase64());
		writer.writeTextElement(pageLevelKey, QString::number(m_page->pageLevel));

		QString storagesStr;
		for (int i = 0; i < m_page->storages.size(); ++i)
		{
			storagesStr = storagesStr % (m_page->storages[i] ? QLatin1Char('1') : QLatin1Char('0'));
		}

		writer.writeTextElement(storagesKey, storagesStr);
		writer.writeEndElement(); // end page
	}

private:
	const ParsedPage* m_page;
	int m_index;
	std::map<const ParsedPage*, int>& m_pagesByIndex;
};

class ParsedPageDeserializer
{
public:
	ParsedPageDeserializer(int index, std::map<int, ParsedPagePtr>& pagesByIndex)
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

			//ResourceType resourceType = ResourceType::ResourceHtml;

			ResourceLink resourceLinOnPage
			{
				ParsedPageWeakPtr(),
				url,
				linkParameter,
				resourceSource,
				altOrTitle
			};


			if (resourceIndex != -1)
			{
				auto it = m_pagesByIndex.find(resourceIndex);
				ASSERT(it != m_pagesByIndex.end());
				resourceLinOnPage.resource = it->second;
				
				ResourceLink resourceLinkToPage = resourceLinOnPage;
				resourceLinkToPage.resource = m_page;
				resourceLinkToPage.url = m_page->url;
				
				it->second->linksToThisPage.push_back(resourceLinkToPage);
			}

			m_page->linksOnThisPage.push_back(resourceLinOnPage);
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

		QString storagesStr = pageMap[storagesKey].toString();
		m_page->storages.resize(storagesStr.size());
		for (int i = 0; i < storagesStr.size(); ++i)
		{
			m_page->storages[i] = storagesStr[i] == QLatin1Char('1');
		}
	}

	ParsedPagePtr page() const
	{
		return m_page;
	}

private:
	ParsedPagePtr m_page;
	int m_index;
	std::map<int, ParsedPagePtr>& m_pagesByIndex;
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

void Serializer::saveToStream(QIODevice& device)
{
	//saveToJsonStream(device);
	saveToXmlStream(device);
}

void Serializer::loadFromStream(QIODevice& device)
{
	//loadFromJsonStream(device);
	loadFromXmlStream(device);
}

const std::vector<ParsedPagePtr>& CrawlerEngine::Serializer::pages() const
{
	return m_deserializedPages;
}

const std::vector<CrawlerRequest>& Serializer::crawledLinks() const
{
	return m_crawledLinks;
}

const std::vector<CrawlerRequest>& CrawlerEngine::Serializer::pendingLinks() const
{
	return m_pendingLinks;
}

void Serializer::saveToJsonStream(QIODevice& device)
{
	Common::JsonParserStreamWriter stream(device);

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

void Serializer::loadFromJsonStream(QIODevice& device)
{
	Common::JsonParserStreamReader stream(device);
	INFOLOG << "Deserialization...";
	Common::JsonStreamMapReader map(stream);
	const int pagesCount = map.readValue(pagesCountKey).toInt();
	readPagesFromJsonStream(map.value(pagesKey), pagesCount);

	readLinksFromJsonStream(map.value(crawledUrlsKey), m_crawledLinks);
	readLinksFromJsonStream(map.value(pendingUrlsKey), m_pendingLinks);

	INFOLOG << "Deserialization has been finished";
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
	INFOLOG << "Deserialization (pages)....";
	Common::JsonStreamListReader reader(stream);
	
	std::vector<ParsedPageDeserializer> pageWrappers;
	pageWrappers.reserve(pagesCount);
	std::map<int, ParsedPagePtr> pagesByIndex;

	int index = 0;
	while (!reader.endOfList())
	{
		QVariantMap pageMap = reader.readCompound().toMap();
		ParsedPageDeserializer& wrapper = pageWrappers.emplace_back(index, pagesByIndex);
		wrapper.fromJson(pageMap);
		++index;
	}

	INFOLOG << "Deserialization (resolving links)....";

	for (ParsedPageDeserializer& wrapper: pageWrappers)
	{
		wrapper.resolveLinks();
		m_deserializedPages.push_back(wrapper.page());
	}

	INFOLOG << "Deserialization (pages) has been finished";
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

void Serializer::saveToXmlStream(QIODevice& device)
{
	QXmlStreamWriter xmlWriter(&device);

	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();

	xmlWriter.writeStartDocument();
	xmlWriter.writeTextElement(serializerVersionKey, serializerVersion);
	xmlWriter.writeTextElement(pagesCountKey, QString::number(m_pages.size()));

	savePagesToXmlStream(xmlWriter);

	xmlWriter.writeStartElement(crawledUrlsKey);
	saveLinksToXmlStream(xmlWriter, m_crawledLinks);
	xmlWriter.writeEndElement();

	xmlWriter.writeStartElement(pendingUrlsKey);
	saveLinksToXmlStream(xmlWriter, m_pendingLinks);
	xmlWriter.writeEndElement();
}

void Serializer::loadFromXmlStream(QIODevice& device)
{
	// TODO: implement
	Q_UNUSED(device);
}

void Serializer::savePagesToXmlStream(QXmlStreamWriter& writer) const
{
	writer.writeStartElement(pagesKey);

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
		wrapper.writeXml(writer);
	}

	writer.writeEndElement(); // end pagesKey
}

void Serializer::saveLinksToXmlStream(QXmlStreamWriter& writer, const std::vector<CrawlerRequest>& links) const
{
	for (const CrawlerRequest& link : links)
	{
		writer.writeStartElement(urlItemKey);
		writer.writeAttribute(urlKey, link.url.toDisplayString());
		writer.writeAttribute(requestTypeKey, QString::number(static_cast<int>(link.requestType)));
		writer.writeEndElement();
	}
}

}



