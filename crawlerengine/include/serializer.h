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
	void saveToJsonStream(Common::JsonParserStreamWriter& stream);
	void readFromJsonStream(Common::JsonParserStreamReader& stream);

	const std::vector<ParsedPagePtr>& pages() const;
	const std::vector<CrawlerRequest>& crawledLinks() const;
	const std::vector<CrawlerRequest>& pendingLinks() const;

private:
	void savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const;
	void readPagesFromJsonStream(Common::JsonParserStreamReader& stream, int pagesCount);

	void saveLinksToJsonStream(Common::JsonParserStreamWriter& stream, const std::vector<CrawlerRequest>& links) const;
	void readLinksFromJsonStream(Common::JsonParserStreamReader& stream, std::vector<CrawlerRequest>& links);

private:
	std::vector<ParsedPage*> m_pages;
	std::vector<ParsedPagePtr> m_deserializedPages;
	std::vector<CrawlerRequest> m_crawledLinks;
	std::vector<CrawlerRequest> m_pendingLinks;
};

}
