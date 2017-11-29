#pragma once

namespace Common 
{
class JsonParserStreamWriter;
}

namespace CrawlerEngine
{
	
class Serializer
{
public:
	void saveToJsonStream(Common::JsonParserStreamWriter& stream) const;

private:
	void savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const;
};

}
