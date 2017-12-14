#pragma once
#include "crawler_request.h"

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
	Serializer(std::vector<ParsedPage*>&& pages, std::vector<CrawlerRequest>&& crawledUrls, std::vector<CrawlerRequest>&& pendingUrls);
	void saveToStream(QIODevice& device);
	void loadFromStream(QIODevice& device);

	const std::vector<ParsedPagePtr>& pages() const;
	const std::vector<CrawlerRequest>& crawledLinks() const;
	const std::vector<CrawlerRequest>& pendingLinks() const;

private:
	void saveToJsonStream(QIODevice& device);
	void loadFromJsonStream(QIODevice& device);
	void savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const;
	void readPagesFromJsonStream(Common::JsonParserStreamReader& stream, int pagesCount);

	void saveLinksToJsonStream(Common::JsonParserStreamWriter& stream, const std::vector<CrawlerRequest>& links) const;
	void readLinksFromJsonStream(Common::JsonParserStreamReader& stream, std::vector<CrawlerRequest>& links);

private:
	void saveToXmlStream(QIODevice& device);
	void loadFromXmlStream(QIODevice& device);
	void savePagesToXmlStream(QXmlStreamWriter& writer) const;
	void loadPagesFromXmlStream(QXmlStreamReader& reader, int pagesCount);

	void saveLinksToXmlStream(QXmlStreamWriter& writer, const std::vector<CrawlerRequest>& links) const;
	void loadLinksFromXmlStream(QXmlStreamReader& reader, std::vector<CrawlerRequest>& links, const QString& xmlElementName);
private:
	std::vector<ParsedPage*> m_pages;
	std::vector<ParsedPagePtr> m_deserializedPages;
	std::vector<CrawlerRequest> m_crawledLinks;
	std::vector<CrawlerRequest> m_pendingLinks;
};

}
