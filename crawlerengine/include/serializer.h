#pragma once

namespace Common 
{
class JsonParserStreamWriter;
}

namespace CrawlerEngine
{
class ISequencedStorage;

class Serializer
{
public:
	Serializer(const ISequencedStorage* crawledPages);
	void saveToJsonStream(Common::JsonParserStreamWriter& stream) const;

private:
	void savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const;

private:
	const ISequencedStorage* m_crawledPages;
};

}
