#include "serializer.h"
#include "json_parser_stream_writer.h"

namespace CrawlerEngine
{

namespace
{
	const QString pagesKey = QLatin1String("pages");

	const QString serializerVersionKey = QLatin1String("serializer_version");
	const QString serializerVersion = QLatin1String("1");
}

void Serializer::saveToJsonStream(Common::JsonParserStreamWriter& stream) const
{
	Common::JsonStreamMapElement map(stream);

	map.writeMapValue(serializerVersionKey, serializerVersion);

	Common::JsonStreamMapValue pagesNode(map, pagesKey);
	savePagesToJsonStream(pagesNode);
}

void Serializer::savePagesToJsonStream(Common::JsonParserStreamWriter& stream) const
{
	Q_UNUSED(stream);
	// TODO: implement
}
	
}



