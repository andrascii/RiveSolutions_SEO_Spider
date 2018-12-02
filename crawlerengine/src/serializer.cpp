#include "serializer.h"
#include "isequenced_storage.h"
#include "unique_link_store.h"

namespace CrawlerEngine
{

namespace
{
	const QString s_contentKey = QLatin1String("content");

	const QString s_pagesCountKey = QLatin1String("pagesCount");
	const QString s_pagesKey = QLatin1String("pages");
	const QString s_pageKey = QLatin1String("page");

	const QString s_serializerVersionKey = QLatin1String("serializer_version");
	const QString s_serializerVersion = QLatin1String("1");

	const QString s_urlKey = QLatin1String("url");
	const QString s_baseUrlKey = QLatin1String("baseUrl");
	const QString s_redirectedUrlKey = QLatin1String("redirectedUrl");
	const QString s_canonicalUrlKey = QLatin1String("canonicalUrl");
	const QString s_titleKey = QLatin1String("title");
	const QString s_contentTypeKey = QLatin1String("contentType");
	const QString s_metaRefreshKey = QLatin1String("metaRefresh");
	const QString s_metaDescriptionKey = QLatin1String("metaDescription");
	const QString s_metaKeywordsKey = QLatin1String("metaKeywords");
	const QString s_serverResponseKey = QLatin1String("serverResponse");
	const QString s_firstH1Key = QLatin1String("firstH1");
	const QString s_secondH1Key = QLatin1String("secondH1");
	const QString s_firstH2Key = QLatin1String("firstH2");
	const QString s_secondH2Key = QLatin1String("secondH2");
	const QString s_statusCodeKey = QLatin1String("statusCode");
	const QString s_metaRobotsFlagsKey = QLatin1String("metaRobotsFlags");
	const QString s_metaRobotsFlagsItemKey = QLatin1String("metaRobotsItem");
	const QString s_metaRobotsValueKey = QLatin1String("metaRobotsValue");
	const QString s_userAgentKey = QLatin1String("userAgent");
	const QString s_responseDateKey = QLatin1String("responseDate");
	const QString s_lastModifiedDateKey = QLatin1String("lastModifiedDate");
	const QString s_pageSizeKilobytesKey = QLatin1String("pageSizeKilobytes");
	const QString s_wordCountKey = QLatin1String("wordCount");
	const QString s_secondsToRefreshKey = QLatin1String("secondsToRefresh");
	const QString s_pageHashKey = QLatin1String("pageHash");
	const QString s_hasSeveralTitleTagsKey = QLatin1String("hasSeveralTitleTags");
	const QString s_hasSeveralMetaDescriptionTagsKey = QLatin1String("hasSeveralMetaDescriptionTags");
	const QString s_hasSeveralMetaKeywordsTagsKey = QLatin1String("hasSeveralMetaKeywordsTags");
	const QString s_hasSeveralH1TagsKey = QLatin1String("hasSeveralH1Tags");
	const QString s_hasSeveralEqualH2TagsKey = QLatin1String("hasSeveralEqualH2Tags");
	const QString s_hasMetaRefreshKey = QLatin1String("hasMetaRefreshKey");
	const QString s_hasFramesKey = QLatin1String("hasFramesKey");
	const QString s_isThisExternalPageKey = QLatin1String("isThisExternalPage");
	const QString s_isBlockedForIndexingKey = QLatin1String("isBlockedForIndexing");
	const QString s_isBlockedByMetaRobotsKey = QLatin1String("isBlockedByMetaRobots");
	const QString s_resourceTypeKey = QLatin1String("resourceType");
	const QString s_rawResponseKey = QLatin1String("rawResponse");
	const QString s_pageLevelKey = QLatin1String("pageLevel");
	const QString s_pageResponseTimeKey = QLatin1String("pageResponseTime");
	const QString s_linksOnThisPageKey = QLatin1String("linksOnThisPage");
	const QString s_linksOnThisPageItemKey = QLatin1String("link");
	const QString s_linkParameterKey = QLatin1String("linkParameter");
	const QString s_resourceSourceKey = QLatin1String("resourceSource");
	const QString s_altOrTitleKey = QLatin1String("altOrTitle");
	const QString s_resourceIndexKey = QLatin1String("resourceIndex");
	const QString s_requestTypeKey = QLatin1String("requestType");
	const QString s_crawledUrlsKey = QLatin1String("crawledUrls");
	const QString s_pendingUrlsKey = QLatin1String("pendingUrls");
	const QString s_urlItemKey = QLatin1String("urlItem");
	const QString s_storagesKey = QLatin1String("storages");

	const QString s_optionsKey = QLatin1String("options");
	const QString s_hostKey = QLatin1String("host");
	const QString s_limitMaxUrlLengthKey = QLatin1String("limitMaxUrlLength");
	const QString s_limitSearchTotalKey = QLatin1String("limitSearchTotal");
	const QString s_limitTimeoutKey = QLatin1String("limitTimeout");
	const QString s_maxRedirectsToFollowKey = QLatin1String("maxRedirectsToFollow");
	const QString s_maxLinksCountOnPageKey = QLatin1String("maxLinksCountOnPage");
	const QString s_minTitleLengthKey = QLatin1String("minTitleLength");
	const QString s_maxTitleLengthKey = QLatin1String("maxTitleLength");
	const QString s_maxDescriptionLengthKey = QLatin1String("maxDescriptionLength");
	const QString s_minDescriptionLengthKey = QLatin1String("minDescriptionLength");
	const QString s_maxH1LengthCharsKey = QLatin1String("maxH1LengthChars");
	const QString s_maxH2LengthCharsKey = QLatin1String("maxH2LengthChars");
	const QString s_maxImageAltTextCharsKey = QLatin1String("maxImageAltTextChars");
	const QString s_maxImageSizeKbKey = QLatin1String("maxImageSizeKb");
	const QString s_maxPageSizeKbKey = QLatin1String("maxPageSizeKb");
	const QString s_useProxyKey = QLatin1String("useProxy");
	const QString s_proxyHostNameKey = QLatin1String("proxyHostName");
	const QString s_proxyPortKey = QLatin1String("proxyPort");
	const QString s_proxyUserKey = QLatin1String("proxyUser");
	const QString s_proxyPasswordKey = QLatin1String("proxyPassword");
	const QString s_checkExternalLinksKey = QLatin1String("checkExternalLinks");
	const QString s_followInternalNofollowKey = QLatin1String("followInternalNofollow");
	const QString s_followExternalNofollowKey = QLatin1String("followExternalNofollow");
	const QString s_checkCanonicalsKey = QLatin1String("checkCanonicals");
	const QString s_checkSubdomainsKey = QLatin1String("checkSubdomains");
	const QString s_crawlOutsideOfStartFolderKey = QLatin1String("crawlOutsideOfStartFolder");
	const QString s_followRobotsTxtRulesKey = QLatin1String("followRobotsTxtRules");
	const QString s_userAgentToFollowKey = QLatin1String("userAgentToFollow");
	const QString s_parserTypeFlagsKey = QLatin1String("parserTypeFlags");
	const QString s_pauseRangeFromKey = QLatin1String("pauseRangeFrom");
	const QString s_pauseRangeToKey = QLatin1String("pauseRangeTo");
	const QString s_pauseRangeEnabledKey = QLatin1String("pauseRangeEnabled");

	const QString s_robotsTxtValidKey = QLatin1String("robotsTxtValid");
	const QString s_robotsTxtContentKey = QLatin1String("robotsTxtContent");
	const QString s_robotsTxtUrlKey = QLatin1String("robotsTxtUrl");
	const QString s_siteMapValidKey = QLatin1String("siteMapValid");
	const QString s_siteMapContentKey = QLatin1String("siteMapContent");
	const QString s_siteMapUrlKey = QLatin1String("siteMapUrl");
	const QString s_is404PagesSetupRightKey = QLatin1String("is404PagesSetupRight");
	const QString s_siteImageKey = QLatin1String("siteImage");

	// missing yandex metrica keys
	const QString s_ymMissingCountersKey = QLatin1String("yandexMetricaMissingCounters");
	const QString s_storageTypeKey = QLatin1String("storageType");
	const QString s_storageTypeValueKey = QLatin1String("storageTypeValue");

	// missing yandex metrica settings keys
	const QString s_searchYandexMetricaCountersSettingsKey = QLatin1String("searchYandexMetricaCounters");

	const QString s_searchYandexMetricaCounter1SettingsKey = QLatin1String("searchYandexMetricaCounter1");
	const QString s_yandexMetricaCounter1IdSettingsKey = QLatin1String("yandexMetricaCounter1Id");

	const QString s_searchYandexMetricaCounter2SettingsKey = QLatin1String("searchYandexMetricaCounter2");
	const QString s_yandexMetricaCounter2IdSettingsKey = QLatin1String("yandexMetricaCounter2Id");

	const QString s_searchYandexMetricaCounter3SettingsKey = QLatin1String("searchYandexMetricaCounter3");
	const QString s_yandexMetricaCounter3IdSettingsKey = QLatin1String("yandexMetricaCounter3Id");

	const QString s_searchYandexMetricaCounter4SettingsKey = QLatin1String("searchYandexMetricaCounter4");
	const QString s_yandexMetricaCounter4IdSettingsKey = QLatin1String("yandexMetricaCounter4Id");

	const QString s_searchYandexMetricaCounter5SettingsKey = QLatin1String("searchYandexMetricaCounter5");
	const QString s_yandexMetricaCounter5IdSettingsKey = QLatin1String("yandexMetricaCounter5Id");

	QString urlToString(const Url& url)
	{
		//return url.toDisplayString(QUrl::FullyEncoded).toUtf8().toBase64();
		return url.toDisplayString(QUrl::FullyEncoded);
	}

	Url urlFromString(const QString& urlStr)
	{
		//return Url(QString::fromUtf8(QByteArray::fromBase64(urlStr.toUtf8())));
		return Url(urlStr);
	}
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
		writer.writeStartElement(s_pageKey);

		writer.writeTextElement(s_urlKey, urlToString(m_page->url));
		writer.writeTextElement(s_baseUrlKey, urlToString(m_page->baseUrl));
		writer.writeTextElement(s_redirectedUrlKey, urlToString(m_page->redirectedUrl));
		writer.writeTextElement(s_canonicalUrlKey, urlToString(m_page->canonicalUrl));
		writer.writeTextElement(s_titleKey, m_page->title);
		writer.writeTextElement(s_contentTypeKey, m_page->contentType);
		writer.writeTextElement(s_metaRefreshKey, m_page->metaRefresh);
		writer.writeTextElement(s_metaDescriptionKey, m_page->metaDescription);
		writer.writeTextElement(s_metaKeywordsKey, m_page->metaKeywords);
		writer.writeTextElement(s_serverResponseKey, m_page->serverResponse);
		writer.writeTextElement(s_firstH1Key, m_page->firstH1);
		writer.writeTextElement(s_secondH1Key, m_page->secondH1);
		writer.writeTextElement(s_firstH2Key, m_page->firstH2);
		writer.writeTextElement(s_secondH2Key, m_page->secondH2);
		writer.writeTextElement(s_statusCodeKey, QString::number(static_cast<int>(m_page->statusCode)));

		{
			writer.writeStartElement(s_metaRobotsFlagsKey);
			for (const std::pair<const UserAgentType, MetaRobotsFlags>& flags : m_page->metaRobotsFlags)
			{
				writer.writeStartElement(s_metaRobotsFlagsItemKey);
				writer.writeAttribute(s_userAgentKey, QString::number(static_cast<int>(flags.first)));
				writer.writeAttribute(s_metaRobotsValueKey, QString::number(static_cast<int>(flags.second)));
				writer.writeEndElement();
			}

			writer.writeEndElement();
		}

		writer.writeTextElement(s_responseDateKey, QString::number(static_cast<unsigned long long>(m_page->responseDate.toMSecsSinceEpoch())));
		writer.writeTextElement(s_lastModifiedDateKey, QString::number(static_cast<unsigned long long>(m_page->lastModifiedDate.toMSecsSinceEpoch())));
		writer.writeTextElement(s_pageSizeKilobytesKey, QString::number(m_page->pageSizeKilobytes));
		writer.writeTextElement(s_wordCountKey, QString::number(m_page->wordCount));
		writer.writeTextElement(s_secondsToRefreshKey, QString::number(m_page->secondsToRefresh));
		writer.writeTextElement(s_pageHashKey, QString::number(m_page->pageHash));
		writer.writeTextElement(s_hasSeveralTitleTagsKey, QString::number(m_page->hasSeveralTitleTags));
		writer.writeTextElement(s_hasSeveralMetaDescriptionTagsKey, QString::number(m_page->hasSeveralMetaDescriptionTags));
		writer.writeTextElement(s_hasSeveralMetaKeywordsTagsKey, QString::number(m_page->hasSeveralMetaKeywordsTags));
		writer.writeTextElement(s_hasSeveralH1TagsKey, QString::number(m_page->hasSeveralH1Tags));
		writer.writeTextElement(s_hasSeveralEqualH2TagsKey, QString::number(m_page->hasSeveralEqualH2Tags));
		writer.writeTextElement(s_hasMetaRefreshKey, QString::number(m_page->hasMetaRefreshTag));
		writer.writeTextElement(s_hasFramesKey, QString::number(m_page->hasFrames));

		writer.writeTextElement(s_isThisExternalPageKey, QString::number(m_page->isThisExternalPage));
		writer.writeTextElement(s_isBlockedForIndexingKey, QString::number(m_page->isBlockedForIndexing));
		writer.writeTextElement(s_isBlockedByMetaRobotsKey, QString::number(m_page->isBlockedByMetaRobots));
		writer.writeTextElement(s_resourceTypeKey, QString::number(static_cast<int>(m_page->resourceType)));

		{
			writer.writeStartElement(s_ymMissingCountersKey);

			for (StorageType storageType : m_page->missingYandexMetricaCounters)
			{
				writer.writeStartElement(s_storageTypeKey);
				writer.writeAttribute(s_storageTypeValueKey, QString::number(static_cast<int>(storageType)));
				writer.writeEndElement();
			}

			writer.writeEndElement();
		}

		{
			writer.writeStartElement(s_linksOnThisPageKey);

			for (const ResourceLink& link : m_page->linksOnThisPage)
			{
				writer.writeStartElement(s_linksOnThisPageItemKey);

				writer.writeAttribute(s_urlKey, urlToString(link.url));
				writer.writeAttribute(s_linkParameterKey, QString::number(static_cast<int>(link.linkParameter)));
				writer.writeAttribute(s_resourceSourceKey, QString::number(static_cast<int>(link.resourceSource)));
				writer.writeAttribute(s_altOrTitleKey, link.altOrTitle);

				if (!link.resource.expired())
				{
					const ParsedPage* linkPage = link.resource.lock().get();
					auto it = m_pagesByIndex.find(linkPage);
#ifdef PRODUCTION
					if (it == m_pagesByIndex.end())
					{
						WARNLOG << "Cannot find" << linkPage->url.urlStr();
						writer.writeAttribute(s_resourceIndexKey, QString::number(-1));
					}
					else
					{
						writer.writeAttribute(s_resourceIndexKey, QString::number(it->second));
					}
#endif
#ifndef PRODUCTION
					ASSERT(it != m_pagesByIndex.end());

					writer.writeAttribute(s_resourceIndexKey, QString::number(it->second));
#endif
				}
				else
				{
					writer.writeAttribute(s_resourceIndexKey, QString::number(-1));
				}

				writer.writeEndElement();
			}

			writer.writeEndElement();
		}

		writer.writeTextElement(s_rawResponseKey, m_page->rawResponse.toBase64());
		writer.writeTextElement(s_pageLevelKey, QString::number(m_page->pageLevel));
		writer.writeTextElement(s_pageResponseTimeKey, QString::number(m_page->responseTime));

		QString storagesStr;
		for (size_t i = 0; i < m_page->storages.size(); ++i)
		{
			storagesStr = storagesStr % (m_page->storages[i] ? QLatin1Char('1') : QLatin1Char('0'));
		}

		writer.writeTextElement(s_storagesKey, storagesStr);
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
		for (size_t i = 0; i < m_linksOnThisPageIndices.size(); ++i)
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
		while (!reader.isEndElement() || reader.qualifiedName() != s_pageKey)
		{
			reader.readNext();

			if (!reader.isStartElement())
			{
				continue;
			}

			if (reader.qualifiedName() == s_urlKey)
			{
				m_page->url = urlFromString(reader.readElementText());
			}
			else if (reader.qualifiedName() == s_baseUrlKey)
			{
				m_page->baseUrl = urlFromString(reader.readElementText());
			}
			else if (reader.qualifiedName() == s_redirectedUrlKey)
			{
				m_page->redirectedUrl = urlFromString(reader.readElementText());
			}
			else if (reader.qualifiedName() == s_canonicalUrlKey)
			{
				m_page->canonicalUrl = urlFromString(reader.readElementText());
			}
			else if (reader.qualifiedName() == s_titleKey)
			{
				m_page->title = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_contentTypeKey)
			{
				m_page->contentType = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_metaRefreshKey)
			{
				m_page->metaRefresh = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_metaDescriptionKey)
			{
				m_page->metaDescription = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_metaKeywordsKey)
			{
				m_page->metaKeywords = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_serverResponseKey)
			{
				m_page->serverResponse = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_firstH1Key)
			{
				m_page->firstH1 = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_secondH1Key)
			{
				m_page->secondH1 = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_firstH2Key)
			{
				m_page->firstH2 = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_secondH2Key)
			{
				m_page->secondH2 = reader.readElementText();
			}
			else if (reader.qualifiedName() == s_statusCodeKey)
			{
				m_page->statusCode = static_cast<Common::StatusCode>(reader.readElementText().toInt());
			}
			else if (reader.qualifiedName() == s_metaRobotsFlagsKey)
			{
				while (!reader.isEndElement() || reader.qualifiedName() != s_metaRobotsFlagsKey)
				{
					reader.readNext();

					if (reader.isStartElement() && reader.qualifiedName() == s_metaRobotsFlagsItemKey)
					{
						QXmlStreamAttributes attributes = reader.attributes();

						const UserAgentType userAgentType = static_cast<UserAgentType>(attributes.value(s_userAgentKey).toInt());
						const MetaRobotsFlags flags = MetaRobotsFlags(attributes.value(s_metaRobotsValueKey).toInt());
						m_page->metaRobotsFlags[userAgentType] = flags;
					}

				}
			}
			else if (reader.qualifiedName() == s_responseDateKey)
			{
				m_page->responseDate = QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong());
			}
			else if (reader.qualifiedName() == s_lastModifiedDateKey)
			{
				m_page->lastModifiedDate = QDateTime::fromMSecsSinceEpoch(reader.readElementText().toULongLong());
			}
			else if (reader.qualifiedName() == s_pageSizeKilobytesKey)
			{
				m_page->pageSizeKilobytes = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == s_wordCountKey)
			{
				m_page->wordCount = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == s_secondsToRefreshKey)
			{
				m_page->secondsToRefresh = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == s_pageHashKey)
			{
				m_page->pageHash = static_cast<size_t>(reader.readElementText().toULong());
			}
			else if (reader.qualifiedName() == s_hasSeveralTitleTagsKey)
			{
				m_page->hasSeveralTitleTags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_hasSeveralMetaDescriptionTagsKey)
			{
				m_page->hasSeveralMetaDescriptionTags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_hasSeveralMetaKeywordsTagsKey)
			{
				m_page->hasSeveralMetaKeywordsTags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_hasSeveralH1TagsKey)
			{
				m_page->hasSeveralH1Tags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_hasSeveralEqualH2TagsKey)
			{
				m_page->hasSeveralEqualH2Tags = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_hasMetaRefreshKey)
			{
				m_page->hasMetaRefreshTag = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_hasFramesKey)
			{
				m_page->hasFrames = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_isThisExternalPageKey)
			{
				m_page->isThisExternalPage = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_isBlockedForIndexingKey)
			{
				m_page->isBlockedForIndexing = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_isBlockedByMetaRobotsKey)
			{
				m_page->isBlockedByMetaRobots = reader.readElementText().toInt() == 1;
			}
			else if (reader.qualifiedName() == s_resourceTypeKey)
			{
				m_page->resourceType = static_cast<ResourceType>(reader.readElementText().toInt());
			}
			else if (reader.qualifiedName() == s_rawResponseKey)
			{
				m_page->rawResponse = QByteArray::fromBase64(reader.readElementText().toUtf8()); // toLocal8Bit ???
			}
			else if (reader.qualifiedName() == s_pageLevelKey)
			{
				m_page->pageLevel = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == s_pageResponseTimeKey)
			{
				m_page->responseTime = reader.readElementText().toInt();
			}
			else if (reader.qualifiedName() == s_ymMissingCountersKey)
			{
				while (!reader.isEndElement() || reader.qualifiedName() != s_ymMissingCountersKey)
				{
					reader.readNext();

					if (!reader.isStartElement() || reader.qualifiedName() != s_storageTypeKey)
					{
						continue;
					}

					QXmlStreamAttributes attributes = reader.attributes();
					const StorageType storageType = static_cast<StorageType>(attributes.value(s_storageTypeValueKey).toInt());

					m_page->missingYandexMetricaCounters.push_back(storageType);
				}
			}
			else if (reader.qualifiedName() == s_linksOnThisPageKey)
			{
				while (!reader.isEndElement() || reader.qualifiedName() != s_linksOnThisPageKey)
				{
					reader.readNext();

					if (reader.isStartElement() && reader.qualifiedName() == s_linksOnThisPageItemKey)
					{
						QXmlStreamAttributes attributes = reader.attributes();

						const Url url = urlFromString(attributes.value(s_urlKey).toString());
						const LinkParameter linkParameter = static_cast<LinkParameter>(attributes.value(s_linkParameterKey).toInt());
						const ResourceSource source = static_cast<ResourceSource>(attributes.value(s_resourceSourceKey).toInt());
						const QString altOrTitle = attributes.value(s_altOrTitleKey).toString();
						const int resourceIndex = attributes.value(s_resourceIndexKey).toInt();

						m_page->linksOnThisPage.push_back(ResourceLink { ParsedPageWeakPtr(), url, linkParameter, source, altOrTitle });
						m_linksOnThisPageIndices.push_back(resourceIndex);
					}
				}
			}
			else if (reader.qualifiedName() == s_storagesKey)
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
	std::vector<CrawlerRequest>&& pendingUrls, const CrawlerOptionsData& optionsData, const WebHostInfo::AllData& webHostInfoData)
	: m_pages(std::move(pages))
	, m_crawledLinks(std::move(crawledUrls))
	, m_pendingLinks(std::move(pendingUrls))
	, m_crawlerOptionsData(optionsData)
	, m_webHostInfoData(webHostInfoData)
{
}

void Serializer::saveToStream(QIODevice& device)
{
	saveToXmlStream(device);
}

void Serializer::loadFromStream(QIODevice& device)
{
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

const CrawlerOptionsData& Serializer::crawlerOptionsData() const
{
	return m_crawlerOptionsData;
}

const WebHostInfo::AllData& Serializer::webHostInfoData() const
{
	return m_webHostInfoData;
}

void Serializer::saveToXmlStream(QIODevice& device)
{
	INFOLOG << "Serialization...";
	QXmlStreamWriter xmlWriter(&device);

	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement(s_contentKey);

	xmlWriter.writeTextElement(s_serializerVersionKey, s_serializerVersion);
	xmlWriter.writeTextElement(s_pagesCountKey, QString::number(m_pages.size()));

	saveOptionsToXmlStream(xmlWriter);
	savePagesToXmlStream(xmlWriter);

	xmlWriter.writeStartElement(s_crawledUrlsKey);
	saveLinksToXmlStream(xmlWriter, m_crawledLinks);
	xmlWriter.writeEndElement();

	xmlWriter.writeStartElement(s_pendingUrlsKey);
	saveLinksToXmlStream(xmlWriter, m_pendingLinks);
	xmlWriter.writeEndElement();

	xmlWriter.writeEndElement();
	xmlWriter.writeEndDocument();
	INFOLOG << "Serialization done.";
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

		if (xmlReader.qualifiedName() == s_optionsKey)
		{
			loadOptionsFromXmlStream(xmlReader);
		}

		if (xmlReader.qualifiedName() == s_pagesCountKey)
		{
			pagesCount = xmlReader.readElementText().toInt();
		}

		if (xmlReader.qualifiedName() == s_pagesKey)
		{
			ASSERT(pagesCount != -1);
			loadPagesFromXmlStream(xmlReader, pagesCount);
		}

		if (xmlReader.qualifiedName() == s_crawledUrlsKey)
		{
			loadLinksFromXmlStream(xmlReader, m_crawledLinks, s_crawledUrlsKey);
		}

		if (xmlReader.qualifiedName() == s_pendingUrlsKey)
		{
			loadLinksFromXmlStream(xmlReader, m_pendingLinks, s_pendingUrlsKey);
		}

	}

	INFOLOG << "Deserialization has been finished";
}

void Serializer::savePagesToXmlStream(QXmlStreamWriter& writer) const
{
	writer.writeStartElement(s_pagesKey);

	std::vector<ParsedPageSerializer> pageWrappers;
	pageWrappers.reserve(m_pages.size());

	std::map<const ParsedPage*, int> pagesByIndex;

	for (int i = 0, size = static_cast<int>(m_pages.size()); i < size; ++i)
	{
		const ParsedPage* page = m_pages[i];

		DEBUGLOG << page->url.urlStr();

		pageWrappers.emplace_back(ParsedPageSerializer(page, i, pagesByIndex));
	}

	for (int i = 0, size = static_cast<int>(m_pages.size()); i < size; ++i)
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

	while (!reader.isEndElement() || reader.qualifiedName() != s_pagesKey)
	{
		reader.readNext();

		if (reader.isStartElement() && reader.qualifiedName() == s_pageKey)
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
		writer.writeStartElement(s_urlItemKey);
		writer.writeAttribute(s_urlKey, urlToString(link.url));
		writer.writeAttribute(s_requestTypeKey, QString::number(static_cast<int>(link.requestType)));
		writer.writeEndElement();
	}
}

void Serializer::loadLinksFromXmlStream(QXmlStreamReader& reader, std::vector<CrawlerRequest>& links, const QString& xmlElementName)
{
	while (!reader.isEndElement() || reader.qualifiedName() != xmlElementName)
	{
		reader.readNext();
		if (reader.isStartElement() && reader.qualifiedName() == s_urlItemKey)
		{
			QXmlStreamAttributes attributes = reader.attributes();
			const Url url = urlFromString(attributes.value(s_urlKey).toString());
			const DownloadRequestType requestType = static_cast<DownloadRequestType>(attributes.value(s_requestTypeKey).toInt());

			links.push_back(CrawlerRequest{ url, requestType });
		}
	}
}

void Serializer::saveOptionsToXmlStream(QXmlStreamWriter& writer) const
{
	writer.writeStartElement(s_optionsKey);

	writer.writeTextElement(s_hostKey, urlToString(m_crawlerOptionsData.startCrawlingPage));
	writer.writeTextElement(s_limitMaxUrlLengthKey, QString::number(m_crawlerOptionsData.limitMaxUrlLength));
	writer.writeTextElement(s_limitSearchTotalKey, QString::number(m_crawlerOptionsData.limitSearchTotal));
	writer.writeTextElement(s_limitTimeoutKey, QString::number(m_crawlerOptionsData.limitTimeout));
	writer.writeTextElement(s_maxRedirectsToFollowKey, QString::number(m_crawlerOptionsData.maxRedirectsToFollow));
	writer.writeTextElement(s_maxLinksCountOnPageKey, QString::number(m_crawlerOptionsData.maxLinksCountOnPage));
	writer.writeTextElement(s_minTitleLengthKey, QString::number(m_crawlerOptionsData.minTitleLength));
	writer.writeTextElement(s_maxTitleLengthKey, QString::number(m_crawlerOptionsData.maxTitleLength));
	writer.writeTextElement(s_maxDescriptionLengthKey, QString::number(m_crawlerOptionsData.maxDescriptionLength));
	writer.writeTextElement(s_minDescriptionLengthKey, QString::number(m_crawlerOptionsData.minDescriptionLength));
	writer.writeTextElement(s_maxH1LengthCharsKey, QString::number(m_crawlerOptionsData.maxH1LengthChars));
	writer.writeTextElement(s_maxH2LengthCharsKey, QString::number(m_crawlerOptionsData.maxH2LengthChars));
	writer.writeTextElement(s_maxImageAltTextCharsKey, QString::number(m_crawlerOptionsData.maxImageAltTextChars));
	writer.writeTextElement(s_maxImageSizeKbKey, QString::number(m_crawlerOptionsData.maxImageSizeKb));
	writer.writeTextElement(s_maxPageSizeKbKey, QString::number(m_crawlerOptionsData.maxPageSizeKb));
	writer.writeTextElement(s_useProxyKey, QString::number(m_crawlerOptionsData.useProxy));
	writer.writeTextElement(s_proxyHostNameKey, m_crawlerOptionsData.proxyHostName);
	writer.writeTextElement(s_proxyPortKey, QString::number(m_crawlerOptionsData.proxyPort));
	writer.writeTextElement(s_proxyUserKey, m_crawlerOptionsData.proxyUser);
	writer.writeTextElement(s_proxyPasswordKey, m_crawlerOptionsData.proxyPassword);
	writer.writeTextElement(s_checkExternalLinksKey, QString::number(m_crawlerOptionsData.checkExternalLinks));
	writer.writeTextElement(s_followInternalNofollowKey, QString::number(m_crawlerOptionsData.followInternalNofollow));
	writer.writeTextElement(s_followExternalNofollowKey, QString::number(m_crawlerOptionsData.followExternalNofollow));
	writer.writeTextElement(s_checkCanonicalsKey, QString::number(m_crawlerOptionsData.checkCanonicals));
	writer.writeTextElement(s_checkSubdomainsKey, QString::number(m_crawlerOptionsData.checkSubdomains));
	writer.writeTextElement(s_crawlOutsideOfStartFolderKey, QString::number(m_crawlerOptionsData.crawlOutsideOfStartFolder));
	writer.writeTextElement(s_followRobotsTxtRulesKey, QString::number(m_crawlerOptionsData.followRobotsTxtRules));
	writer.writeTextElement(s_userAgentToFollowKey, QString::number(static_cast<int>(m_crawlerOptionsData.userAgentToFollow)));
	writer.writeTextElement(s_parserTypeFlagsKey, QString::number(static_cast<int>(m_crawlerOptionsData.parserTypeFlags)));
	writer.writeTextElement(s_pauseRangeFromKey, QString::number(m_crawlerOptionsData.pauseRangeFrom));
	writer.writeTextElement(s_pauseRangeToKey, QString::number(m_crawlerOptionsData.pauseRangeTo));
	writer.writeTextElement(s_pauseRangeEnabledKey, QString::number(m_crawlerOptionsData.pauseRangeEnabled));
	writer.writeTextElement(s_userAgentKey, m_crawlerOptionsData.userAgent);

	writer.writeTextElement(s_searchYandexMetricaCountersSettingsKey, QString::number(static_cast<int>(m_crawlerOptionsData.searchYandexMetricaCounters)));

	writer.writeTextElement(s_searchYandexMetricaCounter1SettingsKey, QString::number(static_cast<int>(m_crawlerOptionsData.searchYandexMetricaCounter1)));
	writer.writeTextElement(s_yandexMetricaCounter1IdSettingsKey, QString::number(m_crawlerOptionsData.yandexMetricaCounter1Id));

	writer.writeTextElement(s_searchYandexMetricaCounter2SettingsKey, QString::number(static_cast<int>(m_crawlerOptionsData.searchYandexMetricaCounter2)));
	writer.writeTextElement(s_yandexMetricaCounter2IdSettingsKey, QString::number(m_crawlerOptionsData.yandexMetricaCounter2Id));

	writer.writeTextElement(s_searchYandexMetricaCounter3SettingsKey, QString::number(static_cast<int>(m_crawlerOptionsData.searchYandexMetricaCounter3)));
	writer.writeTextElement(s_yandexMetricaCounter3IdSettingsKey, QString::number(m_crawlerOptionsData.yandexMetricaCounter3Id));

	writer.writeTextElement(s_searchYandexMetricaCounter4SettingsKey, QString::number(static_cast<int>(m_crawlerOptionsData.searchYandexMetricaCounter4)));
	writer.writeTextElement(s_yandexMetricaCounter4IdSettingsKey, QString::number(m_crawlerOptionsData.yandexMetricaCounter4Id));

	writer.writeTextElement(s_searchYandexMetricaCounter5SettingsKey, QString::number(static_cast<int>(m_crawlerOptionsData.searchYandexMetricaCounter5)));
	writer.writeTextElement(s_yandexMetricaCounter5IdSettingsKey, QString::number(m_crawlerOptionsData.yandexMetricaCounter5Id));


	writer.writeTextElement(s_robotsTxtValidKey, QString::number(
		m_webHostInfoData.isRobotstxtValid != std::nullopt
		? m_webHostInfoData.isRobotstxtValid.value()
		: false
	));

	writer.writeTextElement(s_robotsTxtContentKey, QString::fromUtf8(m_webHostInfoData.robotstxtContent));
	writer.writeTextElement(s_robotsTxtUrlKey, urlToString(m_webHostInfoData.robotstxtUrl));

	writer.writeTextElement(s_siteMapValidKey, QString::number(
		m_webHostInfoData.isSiteMapValid != std::nullopt
		? m_webHostInfoData.isSiteMapValid.value()
		: false
	));

	writer.writeTextElement(s_siteMapContentKey, QString::fromUtf8(m_webHostInfoData.siteMapContent));
	writer.writeTextElement(s_siteMapUrlKey, urlToString(m_webHostInfoData.siteMapUrl));

	writer.writeTextElement(s_is404PagesSetupRightKey, QString::number(
		m_webHostInfoData.is404PagesSetupRight != std::nullopt
		? m_webHostInfoData.is404PagesSetupRight.value()
		: false
	));

	if (!m_webHostInfoData.image.isEmpty())
	{
		writer.writeTextElement(s_siteImageKey, QString::fromUtf8(m_webHostInfoData.image.toBase64()));
	}

	writer.writeEndElement();
}

void Serializer::loadOptionsFromXmlStream(QXmlStreamReader& reader)
{
	while (!reader.isEndElement() || reader.qualifiedName() != s_optionsKey)
	{
		reader.readNext();

		if (!reader.isStartElement())
		{
			continue;
		}

		if (reader.qualifiedName() == s_hostKey)
		{
			m_crawlerOptionsData.startCrawlingPage = urlFromString(reader.readElementText());
		}
		else if (reader.qualifiedName() == s_limitMaxUrlLengthKey)
		{
			m_crawlerOptionsData.limitMaxUrlLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_limitSearchTotalKey)
		{
			m_crawlerOptionsData.limitSearchTotal = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_limitTimeoutKey)
		{
			m_crawlerOptionsData.limitTimeout = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxRedirectsToFollowKey)
		{
			m_crawlerOptionsData.maxRedirectsToFollow = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxLinksCountOnPageKey)
		{
			m_crawlerOptionsData.maxLinksCountOnPage = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_minTitleLengthKey)
		{
			m_crawlerOptionsData.minTitleLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxTitleLengthKey)
		{
			m_crawlerOptionsData.maxTitleLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxDescriptionLengthKey)
		{
			m_crawlerOptionsData.maxDescriptionLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_minDescriptionLengthKey)
		{
			m_crawlerOptionsData.minDescriptionLength = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxH1LengthCharsKey)
		{
			m_crawlerOptionsData.maxH1LengthChars = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxH2LengthCharsKey)
		{
			m_crawlerOptionsData.maxH2LengthChars = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxImageAltTextCharsKey)
		{
			m_crawlerOptionsData.maxImageAltTextChars = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxImageSizeKbKey)
		{
			m_crawlerOptionsData.maxImageSizeKb = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_maxPageSizeKbKey)
		{
			m_crawlerOptionsData.maxPageSizeKb = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_useProxyKey)
		{
			m_crawlerOptionsData.useProxy = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_proxyHostNameKey)
		{
			m_crawlerOptionsData.proxyHostName = reader.readElementText();
		}
		else if (reader.qualifiedName() == s_proxyPortKey)
		{
			m_crawlerOptionsData.proxyPort = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_proxyUserKey)
		{
			m_crawlerOptionsData.proxyUser = reader.readElementText();
		}
		else if (reader.qualifiedName() == s_proxyPasswordKey)
		{
			m_crawlerOptionsData.proxyPassword = reader.readElementText();
		}
		else if (reader.qualifiedName() == s_checkExternalLinksKey)
		{
			m_crawlerOptionsData.checkExternalLinks = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_followInternalNofollowKey)
		{
			m_crawlerOptionsData.followInternalNofollow = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_followExternalNofollowKey)
		{
			m_crawlerOptionsData.followExternalNofollow = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_checkCanonicalsKey)
		{
			m_crawlerOptionsData.checkCanonicals = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_checkSubdomainsKey)
		{
			m_crawlerOptionsData.checkSubdomains = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_crawlOutsideOfStartFolderKey)
		{
			m_crawlerOptionsData.crawlOutsideOfStartFolder = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_followRobotsTxtRulesKey)
		{
			m_crawlerOptionsData.followRobotsTxtRules = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_userAgentToFollowKey)
		{
			m_crawlerOptionsData.userAgentToFollow = static_cast<UserAgentType>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_parserTypeFlagsKey)
		{
			m_crawlerOptionsData.parserTypeFlags = ParserTypeFlags(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_pauseRangeFromKey)
		{
			m_crawlerOptionsData.pauseRangeFrom = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_pauseRangeToKey)
		{
			m_crawlerOptionsData.pauseRangeTo = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_pauseRangeEnabledKey)
		{
			m_crawlerOptionsData.pauseRangeEnabled = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_userAgentKey)
		{
			m_crawlerOptionsData.userAgent = reader.readElementText().toUtf8();
		}
		else if (reader.qualifiedName() == s_searchYandexMetricaCountersSettingsKey)
		{
			m_crawlerOptionsData.searchYandexMetricaCounters = static_cast<bool>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_searchYandexMetricaCounter1SettingsKey)
		{
			m_crawlerOptionsData.searchYandexMetricaCounter1 = static_cast<bool>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_yandexMetricaCounter1IdSettingsKey)
		{
			m_crawlerOptionsData.yandexMetricaCounter1Id = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_searchYandexMetricaCounter2SettingsKey)
		{
			m_crawlerOptionsData.searchYandexMetricaCounter2 = static_cast<bool>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_yandexMetricaCounter2IdSettingsKey)
		{
			m_crawlerOptionsData.yandexMetricaCounter2Id = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_searchYandexMetricaCounter3SettingsKey)
		{
			m_crawlerOptionsData.searchYandexMetricaCounter3 = static_cast<bool>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_yandexMetricaCounter3IdSettingsKey)
		{
			m_crawlerOptionsData.yandexMetricaCounter3Id = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_searchYandexMetricaCounter4SettingsKey)
		{
			m_crawlerOptionsData.searchYandexMetricaCounter4 = static_cast<bool>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_yandexMetricaCounter4IdSettingsKey)
		{
			m_crawlerOptionsData.yandexMetricaCounter4Id = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_searchYandexMetricaCounter5SettingsKey)
		{
			m_crawlerOptionsData.searchYandexMetricaCounter5 = static_cast<bool>(reader.readElementText().toInt());
		}
		else if (reader.qualifiedName() == s_yandexMetricaCounter5IdSettingsKey)
		{
			m_crawlerOptionsData.yandexMetricaCounter5Id = reader.readElementText().toInt();
		}
		else if (reader.qualifiedName() == s_robotsTxtValidKey)
		{
			m_webHostInfoData.isRobotstxtValid = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_robotsTxtContentKey)
		{
			m_webHostInfoData.robotstxtContent = reader.readElementText().toUtf8();
		}
		else if (reader.qualifiedName() == s_robotsTxtUrlKey)
		{
			m_webHostInfoData.robotstxtUrl = urlFromString(reader.readElementText());
		}
		else if (reader.qualifiedName() == s_siteMapValidKey)
		{
			m_webHostInfoData.isSiteMapValid = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_siteMapContentKey)
		{
			m_webHostInfoData.siteMapContent = reader.readElementText().toUtf8();
		}
		else if (reader.qualifiedName() == s_siteMapUrlKey)
		{
			m_webHostInfoData.siteMapUrl = urlFromString(reader.readElementText());
		}
		else if (reader.qualifiedName() == s_is404PagesSetupRightKey)
		{
			m_webHostInfoData.is404PagesSetupRight = reader.readElementText().toInt() == 1;
		}
		else if (reader.qualifiedName() == s_siteImageKey)
		{
			m_webHostInfoData.image = QByteArray::fromBase64(reader.readElementText().toUtf8());
		}
	}
}

}



