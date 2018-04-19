#include "serializer.h"
#include "json_parser_stream_writer.h"
#include "isequenced_storage.h"
#include "json_parser_stream_reader.h"
#include "unique_link_store.h"

namespace CrawlerEngine
{

namespace
{
	const QString contentKey = QLatin1String("content");

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
	const QString secondH2Key = QLatin1String("secondH2");
	const QString statusCodeKey = QLatin1String("statusCode");
	const QString metaRobotsFlagsKey = QLatin1String("metaRobotsFlags");
	const QString metaRobotsFlagsItemKey = QLatin1String("metaRobotsItem");
	const QString metaRobotsValueKey = QLatin1String("metaRobotsValue");
	const QString userAgentKey = QLatin1String("userAgent");
	const QString responseDateKey = QLatin1String("responseDate");
	const QString lastModifiedDateKey = QLatin1String("lastModifiedDate");
	const QString pageSizeKilobytesKey = QLatin1String("pageSizeKilobytes");
	const QString wordCountKey = QLatin1String("wordCount");
	const QString secondsToRefreshKey = QLatin1String("secondsToRefresh");
	const QString pageHashKey = QLatin1String("pageHash");
	const QString hasSeveralTitleTagsKey = QLatin1String("hasSeveralTitleTags");
	const QString hasSeveralMetaDescriptionTagsKey = QLatin1String("hasSeveralMetaDescriptionTags");
	const QString hasSeveralMetaKeywordsTagsKey = QLatin1String("hasSeveralMetaKeywordsTags");
	const QString hasSeveralH1TagsKey = QLatin1String("hasSeveralH1Tags");
	const QString hasSeveralEqualH2TagsKey = QLatin1String("hasSeveralEqualH2Tags");
	const QString hasMetaRefreshKey = QLatin1String("hasMetaRefreshKey");
	const QString hasFramesKey = QLatin1String("hasFramesKey");
	const QString isThisExternalPageKey = QLatin1String("isThisExternalPage");
	const QString isBlockedForIndexingKey = QLatin1String("isBlockedForIndexing");
	const QString isBlockedByMetaRobotsKey = QLatin1String("isBlockedByMetaRobots");
	const QString tooManyRedirectsKey = QLatin1String("tooManyRedirects");
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

	const QString optionsKey = QLatin1String("options");
	const QString hostKey = QLatin1String("host");
	const QString limitMaxUrlLengthKey = QLatin1String("limitMaxUrlLength");
	const QString limitSearchTotalKey = QLatin1String("limitSearchTotal");
	const QString limitTimeoutKey = QLatin1String("limitTimeout");
	const QString maxRedirectsToFollowKey = QLatin1String("maxRedirectsToFollow");
	const QString maxLinksCountOnPageKey = QLatin1String("maxLinksCountOnPage");
	const QString minTitleLengthKey = QLatin1String("minTitleLength");
	const QString maxTitleLengthKey = QLatin1String("maxTitleLength");
	const QString maxDescriptionLengthKey = QLatin1String("maxDescriptionLength");
	const QString minDescriptionLengthKey = QLatin1String("minDescriptionLength");
	const QString maxH1LengthCharsKey = QLatin1String("maxH1LengthChars");
	const QString maxH2LengthCharsKey = QLatin1String("maxH2LengthChars");
	const QString maxImageAltTextCharsKey = QLatin1String("maxImageAltTextChars");
	const QString maxImageSizeKbKey = QLatin1String("maxImageSizeKb");
	const QString maxPageSizeKbKey = QLatin1String("maxPageSizeKb");
	const QString useProxyKey = QLatin1String("useProxy");
	const QString proxyHostNameKey = QLatin1String("proxyHostName");
	const QString proxyPortKey = QLatin1String("proxyPort");
	const QString proxyUserKey = QLatin1String("proxyUser");
	const QString proxyPasswordKey = QLatin1String("proxyPassword");
	const QString checkExternalLinksKey = QLatin1String("checkExternalLinks");
	const QString followInternalNofollowKey = QLatin1String("followInternalNofollow");
	const QString followExternalNofollowKey = QLatin1String("followExternalNofollow");
	const QString checkCanonicalsKey = QLatin1String("checkCanonicals");
	const QString checkSubdomainsKey = QLatin1String("checkSubdomains");
	const QString crawlOutsideOfStartFolderKey = QLatin1String("crawlOutsideOfStartFolder");
	const QString followRobotsTxtRulesKey = QLatin1String("followRobotsTxtRules");
	const QString userAgentToFollowKey = QLatin1String("userAgentToFollow");
	const QString parserTypeFlagsKey = QLatin1String("parserTypeFlags");
	const QString pauseRangeFromKey = QLatin1String("pauseRangeFrom");
	const QString pauseRangeToKey = QLatin1String("pauseRangeTo");
	
	const QString robotsTxtValidKey = QLatin1String("robotsTxtValid");
	const QString robotsTxtContentKey = QLatin1String("robotsTxtContent");
	const QString robotsTxtUrlKey = QLatin1String("robotsTxtUrl");
	const QString siteMapValidKey = QLatin1String("siteMapValid");
	const QString siteMapContentKey = QLatin1String("siteMapContent");
	const QString siteMapUrlKey = QLatin1String("siteMapUrl");
	const QString is404PagesSetupRightKey = QLatin1String("is404PagesSetupRight");
	const QString siteImageKey = QLatin1String("siteImage");


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
		writer.writeTextElement(secondH2Key, m_page->secondH2);
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
		writer.writeTextElement(secondsToRefreshKey, QString::number(m_page->secondsToRefresh));
		writer.writeTextElement(pageHashKey, QString::number(m_page->pageHash));
		writer.writeTextElement(hasSeveralTitleTagsKey, QString::number(m_page->hasSeveralTitleTags));
		writer.writeTextElement(hasSeveralMetaDescriptionTagsKey, QString::number(m_page->hasSeveralMetaDescriptionTags));
		writer.writeTextElement(hasSeveralMetaKeywordsTagsKey, QString::number(m_page->hasSeveralMetaKeywordsTags));
		writer.writeTextElement(hasSeveralH1TagsKey, QString::number(m_page->hasSeveralH1Tags));
		writer.writeTextElement(hasSeveralEqualH2TagsKey, QString::number(m_page->hasSeveralEqualH2Tags));
		writer.writeTextElement(hasMetaRefreshKey, QString::number(m_page->hasMetaRefreshTag));
		writer.writeTextElement(hasFramesKey, QString::number(m_page->hasFrames));

		writer.writeTextElement(isThisExternalPageKey, QString::number(m_page->isThisExternalPage));
		writer.writeTextElement(isBlockedForIndexingKey, QString::number(m_page->isBlockedForIndexing));
		writer.writeTextElement(isBlockedByMetaRobotsKey, QString::number(m_page->isBlockedByMetaRobots));
		writer.writeTextElement(tooManyRedirectsKey, QString::number(m_page->tooManyRedirects));
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
#ifdef PRODUCTION
					if (it == m_pagesByIndex.end())
					{
						WARNLOG << "Cannot find" << linkPage->url.urlStr();
						writer.writeAttribute(resourceIndexKey, QString::number(-1));
					}
#endif
#ifndef PRODUCTION
					ASSERT(it != m_pagesByIndex.end());

					writer.writeAttribute(resourceIndexKey, QString::number(it->second));
#endif
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
		for (int i = 0; i < m_linksOnThisPageIndices.size(); ++i)
		{
			ResourceLink& resourceLinkOnPage = m_page->linksOnThisPage[i];
			const int resourceIndex = m_linksOnThisPageIndices[i];

			if (resourceIndex != -1)
			{
				auto it = m_pagesByIndex.find(resourceIndex);
				ASSERT(it != m_pagesByIndex.end());
				resourceLinkOnPage.resource = it->second;

				ResourceLink resourceLinkToPage = resourceLinkOnPage;
				resourceLinkToPage.resource = m_page;
				resourceLinkToPage.url = m_page->url;

				it->second->linksToThisPage.push_back(resourceLinkToPage);
			}
		}
	}

	void fromXml(QXmlStreamReader& reader)
	{
		while (!reader.isEndElement() || reader.qualifiedName() != pageKey)
		{
			reader.readNext();

			if (!reader.isStartElement())
			{
				continue;
			}

			if (reader.qualifiedName() == urlKey)
			{
				m_page->url = Url(reader.readElementText());
			}
			else if (reader.qualifiedName() == redirectedUrlKey)
			{
				m_page->redirectedUrl = Url(reader.readElementText());
			}
			else if (reader.qualifiedName() == canonicalUrlKey)
			{
				m_page->canonicalUrl = Url(reader.readElementText());
			}
			else if (reader.qualifiedName() == titleKey)
			{
				m_page->title = reader.readElementText();
			}
			else if (reader.qualifiedName() == contentTypeKey)
			{
				m_page->contentType = reader.readElementText();
			}
			else if (reader.qualifiedName() == metaRefreshKey)
			{
				m_page->metaRefresh = reader.readElementText();
			}
			else if (reader.qualifiedName() == metaDescriptionKey)
			{
				m_page->metaDescription = reader.readElementText();
			}
			else if (reader.qualifiedName() == metaKeywordsKey)
			{
				m_page->metaKeywords = reader.readElementText();
			}
			else if (reader.qualifiedName() == serverResponseKey)
			{
				m_page->serverResponse = reader.readElementText();
			}
			else if (reader.qualifiedName() == firstH1Key)
			{
				m_page->firstH1 = reader.readElementText();
			}
			else if (reader.qualifiedName() == secondH1Key)
			{
				m_page->secondH1 = reader.readElementText();
			}
			else if (reader.qualifiedName() == firstH2Key)
			{
				m_page->firstH2 = reader.readElementText();
			}
			else if (reader.qualifiedName() == secondH2Key)
			{
				m_page->secondH2 = reader.readElementText();
			}
			else if (reader.qualifiedName() == statusCodeKey)
			{
				m_page->statusCode = static_cast<Common::StatusCode>(reader.readElementText().toInt());
			}
			else if (reader.qualifiedName() == metaRobotsFlagsKey)
			{
				while (!reader.isEndElement() || reader.qualifiedName() != metaRobotsFlagsKey)
				{
					reader.readNext();

					if (reader.isStartElement() && reader.qualifiedName() == metaRobotsFlagsItemKey)
					{
						QXmlStreamAttributes attributes = reader.attributes();

						const UserAgentType userAgentType = static_cast<UserAgentType>(attributes.value(userAgentKey).toInt());
						const MetaRobotsFlags flags = MetaRobotsFlags(attributes.value(metaRobotsValueKey).toInt());
						m_page->metaRobotsFlags[userAgentType] = flags;
					}

				}
			}
			else if (reader.qualifiedName() == responseDateKey)
			{
				m_page->responseDate = QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong());
			}
			else if (reader.qualifiedName() == lastModifiedDateKey)
			{
				m_page->lastModifiedDate = QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong());
			}
			else if (reader.qualifiedName() == pageSizeKilobytesKey)
			{
				m_page->pageSizeKilobytes = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == wordCountKey)
			{
				m_page->wordCount = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == secondsToRefreshKey)
			{
				m_page->secondsToRefresh = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == pageHashKey)
			{
				m_page->pageHash = static_cast<size_t>(reader.readElementText().toULong());
			}
			else if (reader.qualifiedName() == hasSeveralTitleTagsKey)
			{
				m_page->hasSeveralTitleTags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == hasSeveralMetaDescriptionTagsKey)
			{
				m_page->hasSeveralMetaDescriptionTags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == hasSeveralMetaKeywordsTagsKey)
			{
				m_page->hasSeveralMetaKeywordsTags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == hasSeveralH1TagsKey)
			{
				m_page->hasSeveralH1Tags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == hasSeveralEqualH2TagsKey)
			{
				m_page->hasSeveralEqualH2Tags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == hasMetaRefreshKey)
			{
				m_page->hasMetaRefreshTag = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == hasFramesKey)
			{
				m_page->hasFrames = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == isThisExternalPageKey)
			{
				m_page->isThisExternalPage = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == isBlockedForIndexingKey)
			{
				m_page->isBlockedForIndexing = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == isBlockedByMetaRobotsKey)
			{
				m_page->isBlockedByMetaRobots = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == tooManyRedirectsKey)
			{
				m_page->tooManyRedirects = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == resourceTypeKey)
			{
				m_page->resourceType = static_cast<ResourceType>(reader.readElementText().toInt());
			}
			else if (reader.qualifiedName() == rawResponseKey)
			{
				m_page->rawResponse = QByteArray::fromBase64(reader.readElementText().toUtf8()); // toLocal8Bit ???
			}
			else if (reader.qualifiedName() == pageLevelKey)
			{
				m_page->pageLevel = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == linksOnThisPageKey)
			{
				while (!reader.isEndElement() || reader.qualifiedName() != linksOnThisPageKey)
				{
					reader.readNext();

					if (reader.isStartElement() && reader.qualifiedName() == linksOnThisPageItemKey)
					{
						QXmlStreamAttributes attributes = reader.attributes();

						const Url url = Url(attributes.value(urlKey).toString());
						const LinkParameter linkParameter = static_cast<LinkParameter>(attributes.value(linkParameterKey).toInt());
						const ResourceSource source = static_cast<ResourceSource>(attributes.value(resourceSourceKey).toInt());
						const QString altOrTitle = attributes.value(altOrTitleKey).toString();
						const int resourceIndex = attributes.value(resourceIndexKey).toInt();

						m_page->linksOnThisPage.push_back(ResourceLink { ParsedPageWeakPtr(), url, linkParameter, source, altOrTitle });
						m_linksOnThisPageIndices.push_back(resourceIndex);
					}
				}
			}
			else if (reader.qualifiedName() == storagesKey)
			{
				QString storagesStr = reader.readElementText();
				m_page->storages.resize(storagesStr.size());
				for (int i = 0; i < storagesStr.size(); ++i)
				{
					m_page->storages[i] = storagesStr[i] == QLatin1Char('1');
				}
			}
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
	std::vector<int> m_linksOnThisPageIndices;
};

Serializer::Serializer()
{
}

Serializer::Serializer(std::vector<ParsedPage*>&& pages, std::vector<CrawlerRequest>&& crawledUrls, 
	std::vector<CrawlerRequest>&& pendingUrls, const CrawlerOptions& options, const WebHostInfo::AllData& webHostInfoData)
	: m_pages(std::move(pages))
	, m_crawledLinks(std::move(crawledUrls))
	, m_pendingLinks(std::move(pendingUrls))
	, m_options(options)
	, m_webHostInfoData(webHostInfoData)
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

const CrawlerOptions& Serializer::crawlerOptions() const
{
	return m_options;
}

const WebHostInfo::AllData& Serializer::webHostInfoData() const
{
	return m_webHostInfoData;
}

void Serializer::saveToXmlStream(QIODevice& device)
{
	QXmlStreamWriter xmlWriter(&device);

	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement(contentKey);

	xmlWriter.writeTextElement(serializerVersionKey, serializerVersion);
	xmlWriter.writeTextElement(pagesCountKey, QString::number(m_pages.size()));

	saveOptionsToXmlStream(xmlWriter);
	savePagesToXmlStream(xmlWriter);

	xmlWriter.writeStartElement(crawledUrlsKey);
	saveLinksToXmlStream(xmlWriter, m_crawledLinks);
	xmlWriter.writeEndElement();

	xmlWriter.writeStartElement(pendingUrlsKey);
	saveLinksToXmlStream(xmlWriter, m_pendingLinks);
	xmlWriter.writeEndElement();

	xmlWriter.writeEndElement();
	xmlWriter.writeEndDocument();
}

void Serializer::loadFromXmlStream(QIODevice& device)
{
	QXmlStreamReader xmlReader(&device);

	INFOLOG << "Deserialization...";

	int pagesCount = -1;

	while (!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if (!xmlReader.isStartElement())
		{
			continue;
		}

		if (xmlReader.qualifiedName() == optionsKey)
		{
			loadOptionsFromXmlStream(xmlReader);
		}

		if (xmlReader.qualifiedName() == pagesCountKey)
		{
			pagesCount = xmlReader.readElementText().toInt();
		}

		if (xmlReader.qualifiedName() == pagesKey)
		{
			ASSERT(pagesCount != -1);
			loadPagesFromXmlStream(xmlReader, pagesCount);
		}

		if (xmlReader.qualifiedName() == crawledUrlsKey)
		{
			loadLinksFromXmlStream(xmlReader, m_crawledLinks, crawledUrlsKey);
		}

		if (xmlReader.qualifiedName() == pendingUrlsKey)
		{
			loadLinksFromXmlStream(xmlReader, m_pendingLinks, pendingUrlsKey);
		}

	}

	INFOLOG << "Deserialization has been finished";
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

void Serializer::loadPagesFromXmlStream(QXmlStreamReader& reader, int pagesCount)
{
	INFOLOG << "Deserialization (pages)....";

	std::vector<ParsedPageDeserializer> pageWrappers;
	pageWrappers.reserve(pagesCount);
	std::map<int, ParsedPagePtr> pagesByIndex;
	int index = 0;

	while (!reader.isEndElement() || reader.qualifiedName() != pagesKey)
	{
		reader.readNext();

		if (reader.isStartElement() && reader.qualifiedName() == pageKey)
		{
			ParsedPageDeserializer& wrapper = pageWrappers.emplace_back(index, pagesByIndex);
			wrapper.fromXml(reader);
			++index;
		}
	}

	INFOLOG << "Deserialization (resolving links)....";

	for (ParsedPageDeserializer& wrapper : pageWrappers)
	{
		wrapper.resolveLinks();
		m_deserializedPages.push_back(wrapper.page());
	}

	INFOLOG << "Deserialization (pages) has been finished";
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

void Serializer::loadLinksFromXmlStream(QXmlStreamReader& reader, std::vector<CrawlerRequest>& links, const QString& xmlElementName)
{
	while (!reader.isEndElement() || reader.qualifiedName() != xmlElementName)
	{
		reader.readNext();
		if (reader.isStartElement() && reader.qualifiedName() == urlItemKey)
		{
			QXmlStreamAttributes attributes = reader.attributes();
			const Url url = Url(attributes.value(urlKey).toString());
			const DownloadRequestType requestType = static_cast<DownloadRequestType>(attributes.value(requestTypeKey).toInt());

			links.push_back(CrawlerRequest{ url, requestType });
		}
	}
}

void Serializer::saveOptionsToXmlStream(QXmlStreamWriter& writer) const
{
	writer.writeStartElement(optionsKey);

	writer.writeTextElement(hostKey, m_options.startCrawlingPage.toDisplayString());
	writer.writeTextElement(limitMaxUrlLengthKey, QString::number(m_options.limitMaxUrlLength));
	writer.writeTextElement(limitSearchTotalKey, QString::number(m_options.limitSearchTotal));
	writer.writeTextElement(limitTimeoutKey, QString::number(m_options.limitTimeout));
	writer.writeTextElement(maxRedirectsToFollowKey, QString::number(m_options.maxRedirectsToFollow));
	writer.writeTextElement(maxLinksCountOnPageKey, QString::number(m_options.maxLinksCountOnPage));
	writer.writeTextElement(minTitleLengthKey, QString::number(m_options.minTitleLength));
	writer.writeTextElement(maxTitleLengthKey, QString::number(m_options.maxTitleLength));
	writer.writeTextElement(maxDescriptionLengthKey, QString::number(m_options.maxDescriptionLength));
	writer.writeTextElement(minDescriptionLengthKey, QString::number(m_options.minDescriptionLength));
	writer.writeTextElement(maxH1LengthCharsKey, QString::number(m_options.maxH1LengthChars));
	writer.writeTextElement(maxH2LengthCharsKey, QString::number(m_options.maxH2LengthChars));
	writer.writeTextElement(maxImageAltTextCharsKey, QString::number(m_options.maxImageAltTextChars));
	writer.writeTextElement(maxImageSizeKbKey, QString::number(m_options.maxImageSizeKb));
	writer.writeTextElement(maxPageSizeKbKey, QString::number(m_options.maxPageSizeKb));
	writer.writeTextElement(useProxyKey, QString::number(m_options.useProxy));
	writer.writeTextElement(proxyHostNameKey, m_options.proxyHostName);
	writer.writeTextElement(proxyPortKey, QString::number(m_options.proxyPort));
	writer.writeTextElement(proxyUserKey, m_options.proxyUser);
	writer.writeTextElement(proxyPasswordKey, m_options.proxyPassword);
	writer.writeTextElement(checkExternalLinksKey, QString::number(m_options.checkExternalLinks));
	writer.writeTextElement(followInternalNofollowKey, QString::number(m_options.followInternalNofollow));
	writer.writeTextElement(followExternalNofollowKey, QString::number(m_options.followExternalNofollow));
	writer.writeTextElement(checkCanonicalsKey, QString::number(m_options.checkCanonicals));
	writer.writeTextElement(checkSubdomainsKey, QString::number(m_options.checkSubdomains));
	writer.writeTextElement(crawlOutsideOfStartFolderKey, QString::number(m_options.crawlOutsideOfStartFolder));
	writer.writeTextElement(followRobotsTxtRulesKey, QString::number(m_options.followRobotsTxtRules));
	writer.writeTextElement(userAgentToFollowKey, QString::number(static_cast<int>(m_options.userAgentToFollow)));
	writer.writeTextElement(parserTypeFlagsKey, QString::number(static_cast<int>(m_options.parserTypeFlags)));
	writer.writeTextElement(pauseRangeFromKey, QString::number(m_options.pauseRangeFrom));
	writer.writeTextElement(pauseRangeToKey, QString::number(m_options.pauseRangeTo));
	writer.writeTextElement(userAgentKey, m_options.userAgent);

	
	writer.writeTextElement(robotsTxtValidKey, QString::number(
		m_webHostInfoData.isRobotstxtValid != std::nullopt
		? m_webHostInfoData.isRobotstxtValid.value()
		: false
	));

	writer.writeTextElement(robotsTxtContentKey, QString::fromUtf8(m_webHostInfoData.robotstxtContent));
	writer.writeTextElement(robotsTxtUrlKey, m_webHostInfoData.robotstxtUrl.toDisplayString());

	writer.writeTextElement(siteMapValidKey, QString::number(
		m_webHostInfoData.isSiteMapValid != std::nullopt
		? m_webHostInfoData.isSiteMapValid.value()
		: false
	));

	writer.writeTextElement(siteMapContentKey, QString::fromUtf8(m_webHostInfoData.siteMapContent));
	writer.writeTextElement(siteMapUrlKey, m_webHostInfoData.siteMapUrl.toDisplayString());

	writer.writeTextElement(is404PagesSetupRightKey, QString::number(
		m_webHostInfoData.is404PagesSetupRight != std::nullopt
		? m_webHostInfoData.is404PagesSetupRight.value()
		: false
	));
	
	if (!m_webHostInfoData.image.isNull())
	{
		QByteArray pixmapData;
		QBuffer buffer(&pixmapData);

		const QPixmap& pixmap = qvariant_cast<QPixmap>(m_webHostInfoData.image);
		pixmap.save(&buffer, "PNG");
		writer.writeTextElement(siteImageKey, QString::fromUtf8(pixmapData.toBase64()));
	}

	writer.writeEndElement();
}

void Serializer::loadOptionsFromXmlStream(QXmlStreamReader& reader)
{
	while (!reader.isEndElement() || reader.qualifiedName() != optionsKey)
	{
		reader.readNext();

		if (!reader.isStartElement())
		{
			continue;
		}

		if (reader.qualifiedName() == hostKey)
		{
			m_options.startCrawlingPage = Url(reader.readElementText());
		}
		else if (reader.qualifiedName() == limitMaxUrlLengthKey)
		{
			m_options.limitMaxUrlLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == limitSearchTotalKey)
		{
			m_options.limitSearchTotal = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == limitTimeoutKey)
		{
			m_options.limitTimeout = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxRedirectsToFollowKey)
		{
			m_options.maxRedirectsToFollow = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxLinksCountOnPageKey)
		{
			m_options.maxLinksCountOnPage = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == minTitleLengthKey)
		{
			m_options.minTitleLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxTitleLengthKey)
		{
			m_options.maxTitleLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxDescriptionLengthKey)
		{
			m_options.maxDescriptionLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == minDescriptionLengthKey)
		{
			m_options.minDescriptionLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxH1LengthCharsKey)
		{
			m_options.maxH1LengthChars = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxH2LengthCharsKey)
		{
			m_options.maxH2LengthChars = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxImageAltTextCharsKey)
		{
			m_options.maxImageAltTextChars = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxImageSizeKbKey)
		{
			m_options.maxImageSizeKb = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == maxPageSizeKbKey)
		{
			m_options.maxPageSizeKb = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == useProxyKey)
		{
			m_options.useProxy = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == proxyHostNameKey)
		{
			m_options.proxyHostName = reader.readElementText();
		}
		else if (reader.qualifiedName() == proxyPortKey)
		{
			m_options.proxyPort = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == proxyUserKey)
		{
			m_options.proxyUser = reader.readElementText();
		}
		else if (reader.qualifiedName() == proxyPasswordKey)
		{
			m_options.proxyPassword = reader.readElementText();
		}
		else if (reader.qualifiedName() == checkExternalLinksKey)
		{
			m_options.checkExternalLinks = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == followInternalNofollowKey)
		{
			m_options.followInternalNofollow = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == followExternalNofollowKey)
		{
			m_options.followExternalNofollow = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == checkCanonicalsKey)
		{
			m_options.checkCanonicals = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == checkSubdomainsKey)
		{
			m_options.checkSubdomains = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == crawlOutsideOfStartFolderKey)
		{
			m_options.crawlOutsideOfStartFolder = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == followRobotsTxtRulesKey)
		{
			m_options.followRobotsTxtRules = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == userAgentToFollowKey)
		{
			m_options.userAgentToFollow = static_cast<UserAgentType>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == parserTypeFlagsKey)
		{
			m_options.parserTypeFlags = ParserTypeFlags(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == pauseRangeFromKey)
		{
			m_options.pauseRangeFrom = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == pauseRangeToKey)
		{
			m_options.pauseRangeTo = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == userAgentKey)
		{
			m_options.userAgent = reader.readElementText().toUtf8();
		}
		else if (reader.qualifiedName() == robotsTxtValidKey)
		{
			m_webHostInfoData.isRobotstxtValid = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == robotsTxtContentKey)
		{
			m_webHostInfoData.robotstxtContent = reader.readElementText().toUtf8();
		}
		else if (reader.qualifiedName() == robotsTxtUrlKey)
		{
			m_webHostInfoData.robotstxtUrl = Url(reader.readElementText());
		}
		else if (reader.qualifiedName() == siteMapValidKey)
		{
			m_webHostInfoData.isSiteMapValid = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == siteMapContentKey)
		{
			m_webHostInfoData.siteMapContent = reader.readElementText().toUtf8();
		}
		else if (reader.qualifiedName() == siteMapUrlKey)
		{
			m_webHostInfoData.siteMapUrl = Url(reader.readElementText());
		}
		else if (reader.qualifiedName() == is404PagesSetupRightKey)
		{
			m_webHostInfoData.is404PagesSetupRight = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == siteImageKey)
		{
			m_webHostInfoData.image.loadFromData(QByteArray::fromBase64(reader.readElementText().toUtf8()), "PNG");
		}
	}
}

}



