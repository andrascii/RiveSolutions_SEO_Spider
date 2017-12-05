#pragma once

namespace Common 
{
class JsonParserStreamWriter;
class JsonParserStreamReader;
}

namespace CrawlerEngine
{
class ISequencedStorage;
struct ParsedPage;

class Serializer
{
public:
	Serializer();
	Serializer(const ISequencedStorage* crawledPages);
	void saveToJsonStream(Common::JsonParserStreamWriter& stream);
	void readFromJsonStream(Common::JsonParserStreamReader& stream);

	const std::vector<ParsedPage*>& deserializedPages() const;

private:
	void savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const;
	void readPagesFromJsonStream(Common::JsonParserStreamReader& stream, int pagesCount);

private:
	const ISequencedStorage* m_crawledPages;
	std::vector<ParsedPage*> m_deserializedPages;
};

}
