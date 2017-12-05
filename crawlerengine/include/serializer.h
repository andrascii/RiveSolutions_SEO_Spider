#pragma once
#include "crawler_request.h"

namespace Common 
{
class JsonParserStreamWriter;
class JsonParserStreamReader;
}

namespace CrawlerEngine
{
class UniqueLinkStore;
class ISequencedStorage;
struct ParsedPage;

class Serializer
{
public:
	Serializer();
	Serializer(const ISequencedStorage* crawledPages, const UniqueLinkStore* linkStore);
	void saveToJsonStream(Common::JsonParserStreamWriter& stream);
	void readFromJsonStream(Common::JsonParserStreamReader& stream);

	const std::vector<ParsedPage*>& deserializedPages() const;

private:
	void savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const;
	void readPagesFromJsonStream(Common::JsonParserStreamReader& stream, int pagesCount);

	void saveLinksToJsonStream(Common::JsonParserStreamWriter& stream, const std::vector<CrawlerRequest>& links) const;

private:
	const ISequencedStorage* m_crawledPages;
	const UniqueLinkStore* m_linkStore;
	std::vector<ParsedPage*> m_deserializedPages;
};

}
