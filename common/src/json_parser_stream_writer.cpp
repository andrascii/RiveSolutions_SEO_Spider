#include "json_parser_stream_writer.h"
#include "json_parser.h"

namespace Common
{

	struct JsonParserStreamWriterData
	{
		JsonParserStreamWriterData(QIODevice& output, bool noIndentation) 
			: output(output)
			, useIndentation(!noIndentation)
		{

		}

		enum LevelType
		{			
			List,
			Map
		};
		struct Level
		{
			Level() : type(List), count(0), mapValueStarted(false) {}
			Level(LevelType t, int count = 0) : type(t), count(count), mapValueStarted(false) {}
			LevelType type;
			int count;
			bool mapValueStarted;
		};
		QIODevice& output;
		bool useIndentation;
		QStack<Level> levels;
	};

JsonParserStreamWriter::JsonParserStreamWriter(QIODevice& output, bool noIndentation)
	: m_data(new JsonParserStreamWriterData(output, noIndentation))
{

}

JsonParserStreamWriter::JsonParserStreamWriter(const JsonParserStreamWriter& stream)
	: m_data(stream.m_data)
{

}

int JsonParserStreamWriter::prepareToWrite(const QVariant& v)
{
	if (d()->levels.isEmpty())
	{
		return 0;
	}

	if (d()->levels.top().type == JsonParserStreamWriterData::List)
	{
		return JsonParser::prepareToWriteListItem(d()->output, v, increaseLevelItemsCount(),
			d()->levels.count(), d()->useIndentation);
	}
	
	if (!d()->levels.top().mapValueStarted)
	{
		ASSERT(!"Map value key is not created");
	}
	return JsonParser::prepareToWriteMapValue(d()->output, v, d()->levels.count(), d()->useIndentation);	
}

void JsonParserStreamWriter::writeValue(const QVariant& v)
{
	ASSERT(v.type() != QMetaType::QVariantMap && "Use an appropriate method to write map");
	ASSERT(v.type() != QMetaType::QVariantList && "Use an appropriate method to write list");

	int itemIndent = prepareToWrite(v);

	JsonParser::writeJson(v, d()->output, itemIndent, d()->useIndentation);
}

void JsonParserStreamWriter::beginMap()
{	
	insertTabs(prepareToWrite(QVariantMap()));
	JsonParser::startMap(d()->output, d()->useIndentation);
	d()->levels.push(JsonParserStreamWriterData::Map);
}

void JsonParserStreamWriter::endMap()
{
	ASSERT(d()->levels.pop().type == JsonParserStreamWriterData::Map && "Map is not started");
	JsonParser::endMap(d()->output, d()->levels.count(), d()->useIndentation);
}

void JsonParserStreamWriter::beginList()
{
	insertTabs(prepareToWrite(QVariantList()));
	JsonParser::startList(d()->output);
	d()->levels.push(JsonParserStreamWriterData::List);
}

void JsonParserStreamWriter::endList()
{
	ASSERT(d()->levels.pop().type == JsonParserStreamWriterData::List && "List is not started");
	JsonParser::endList(d()->output, d()->levels.count(), d()->useIndentation);
}

void JsonParserStreamWriter::beginMapValue(const QString& key)
{
	ASSERT(d()->levels.top().type == JsonParserStreamWriterData::Map && "Call beginMap() first");
	ASSERT(!d()->levels.top().mapValueStarted && "Previous map value is not ended");
	d()->levels.top().mapValueStarted = true;
	JsonParser::prepareMapValueKey(d()->output, key, increaseLevelItemsCount(), 
		d()->levels.count(), d()->useIndentation);
}

void JsonParserStreamWriter::endMapValue()
{
	ASSERT(d()->levels.top().type == JsonParserStreamWriterData::Map && "Call beginMap() first");
	ASSERT(d()->levels.top().mapValueStarted && "Map value is not started");
	d()->levels.top().mapValueStarted = false;
}

void JsonParserStreamWriter::writeMapValue(const QString& key, const QVariant& v)
{
	beginMapValue(key);
	writeValue(v);
	endMapValue();
}

void JsonParserStreamWriter::insertTabs(int count)
{
	if (!d()->useIndentation)
	{
		return;
	}

	const QByteArray tabs(count, '\t');
	d()->output.write(tabs);
}

bool JsonParserStreamWriter::increaseLevelItemsCount()
{
	return d()->levels.top().count++ != 0;
}

void JsonParserStreamWriter::writeCompound(const QVariant& v)
{	
	JsonParser::writeJson(v, d()->output, prepareToWrite(v), d()->useIndentation);
}

JsonParserStreamWriterData* JsonParserStreamWriter::d() const
{
	return m_data.get();
}

JsonStreamMapValue::JsonStreamMapValue(JsonStreamMapElement& stream, const QString& key) 
	: JsonParserStreamWriter(stream)
{
	beginMapValue(key);
}

JsonStreamMapValue::~JsonStreamMapValue()
{
	endMapValue();
}


JsonStreamMapElement::JsonStreamMapElement(JsonParserStreamWriter& stream) 
	: JsonParserStreamWriter(stream)
	, m_continuePrevMap(!d()->levels.isEmpty() && 
		d()->levels.top().type == JsonParserStreamWriterData::Map &&
		!d()->levels.top().mapValueStarted)
{
	if (!m_continuePrevMap)
	{
		beginMap();
	}	
}

JsonStreamMapElement::~JsonStreamMapElement()
{
	if (!m_continuePrevMap)
	{
		endMap();
	}
}


JsonStreamListElement::JsonStreamListElement(JsonParserStreamWriter& stream) 
	: JsonParserStreamWriter(stream)
{
	beginList();
}

JsonStreamListElement::~JsonStreamListElement()
{
	endList();
}

}
