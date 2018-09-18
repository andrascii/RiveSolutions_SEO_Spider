#pragma once
#include "crawler_request.h"
#include "crawler_options.h"
#include "web_host_info.h"

namespace Common
{
class JsonParserStreamWriter;
class JsonParserStreamReader;
}

namespace CrawlerEngine
{
struct ParsedPage;

class Serializer
{
public:
	Serializer();

	Serializer(std::vector<ParsedPage*>&& pages, std::vector<CrawlerRequest>&& crawledUrls,
		std::vector<CrawlerRequest>&& pendingUrls, const CrawlerOptionsData& optionsData,
		const WebHostInfo::AllData& webHostInfoData
	);

	void saveToStream(QIODevice& device);
	void loadFromStream(QIODevice& device);

	const std::vector<ParsedPagePtr>& pages() const;
	const std::vector<CrawlerRequest>& crawledLinks() const;
	const std::vector<CrawlerRequest>& pendingLinks() const;
	const CrawlerOptionsData& crawlerOptionsData() const;
	const WebHostInfo::AllData& webHostInfoData() const;

private:
	void saveToXmlStream(QIODevice& device);
	void loadFromXmlStream(QIODevice& device);
	void savePagesToXmlStream(QXmlStreamWriter& writer) const;
	void loadPagesFromXmlStream(QXmlStreamReader& reader, int pagesCount);

	void saveLinksToXmlStream(QXmlStreamWriter& writer, const std::vector<CrawlerRequest>& links) const;
	void loadLinksFromXmlStream(QXmlStreamReader& reader, std::vector<CrawlerRequest>& links, const QString& xmlElementName);

	void saveOptionsToXmlStream(QXmlStreamWriter& writer) const;
	void loadOptionsFromXmlStream(QXmlStreamReader& reader);
private:
	std::vector<ParsedPage*> m_pages;
	std::vector<ParsedPagePtr> m_deserializedPages;
	std::vector<CrawlerRequest> m_crawledLinks;
	std::vector<CrawlerRequest> m_pendingLinks;
	CrawlerOptionsData m_crawlerOptionsData;
	WebHostInfo::AllData m_webHostInfoData;
};

}
