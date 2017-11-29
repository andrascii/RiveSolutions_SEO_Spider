#include "json_parser_stream_reader.h"
#include "json_parser.h"

namespace Common
{

	struct JsonParserStreamReaderData
	{
		JsonParserStreamReaderData(QIODevice& input) 
			: input(input)			
		{
			ASSERT(!input.isSequential());
			ASSERT(input.pos() == 0);

			fillMapsInfo();
		}

		void skipComma()
		{
			JsonParser::skipJson(input);
			if (JsonParser::tryReadChar(input, ','))
			{
				JsonParser::skipJson(input);
			}			
		}

		typedef qint64 Offset;		

		QMap<Offset, QMap<QString, Offset>> maps;
		QMap<Offset, Offset> endOfMaps;
		QMap<Offset, Offset> endOfLists;
		QIODevice& input;

		QStack<Offset> mapsStack;

	private:
		bool skipToMap()
		{
			char c;
			while(input.getChar(&c))		
			{
				if (c == '{')
				{
					input.ungetChar(c);
					return true;
				}				
			}
			return false;
		}

		void skipValue()
		{			
			bool inQuotes = false;
			int sqBracketCount = 0;
			int curlyBracketCount = 0;

			char c;
			bool skipNextSymbol = false;

			while (input.getChar(&c))
			{
				if (inQuotes)
				{
					if (skipNextSymbol)
					{
						skipNextSymbol = false;
						continue;
					}

					if (c == '\\')
					{
						skipNextSymbol = true;
						continue;
					}
				}

				if (c == '\"')
				{
					inQuotes = !inQuotes;
				}

				if (!inQuotes)
				{
					if (c == '{')
					{
						curlyBracketCount++;
					}
					else if (c == '}')
					{
						curlyBracketCount--;
					}
					else if (c == '[')
					{
						sqBracketCount++;
					}
					else if (c == ']')
					{
						sqBracketCount--;
					}
					else if (c == ',' && sqBracketCount == 0 && curlyBracketCount == 0)
					{
						break;
					}

					if (curlyBracketCount < 0)
					{
						DEBUG_ASSERT(curlyBracketCount == -1 && sqBracketCount == 0);
						input.ungetChar(c);
						break;
					}
					else if (sqBracketCount < 0)
					{
						DEBUG_ASSERT(sqBracketCount == -1 && curlyBracketCount == 0);
						input.ungetChar(c);
						break;
					}
				}	
			}

			skipComma();
		}

		void fillList()
		{
			const Offset offset = input.pos();			

			VERIFY(JsonParser::tryReadChar(input, '['));

			JsonParser::skipJson(input);

			if (JsonParser::tryReadChar(input, ']'))
			{
				endOfLists[offset] = input.pos();
				return;
			}

			while (!input.atEnd() && !JsonParser::tryReadChar(input, ']'))
			{
				if (JsonParser::checkChar(input, '{'))
				{
					fillMap();
				}
				else if (JsonParser::checkChar(input, '['))
				{
					fillList();
				}
				else
				{
					skipValue();
				}
			}			

			skipComma();

			endOfLists[offset] = input.pos();
		}

		void fillMap()
		{
			const Offset mapOffset = input.pos();
			maps[mapOffset] = QMap<QString, Offset>();

			VERIFY(JsonParser::tryReadChar(input, '{'));

			JsonParser::skipJson(input);

			if (JsonParser::tryReadChar(input, '}'))
			{
				endOfMaps[mapOffset] = input.pos();
				return;
			}

			while (!input.atEnd() && !JsonParser::tryReadChar(input, '}'))
			{
				const QString key = JsonParser::readMapKey(input);
				maps[mapOffset][key] = input.pos();

				if (JsonParser::checkChar(input, '{'))
				{
					fillMap();
				}
				else if (JsonParser::checkChar(input, '['))
				{
					fillList();
				}
				else
				{
					skipValue();
				}

				skipComma();
			}

			endOfMaps[mapOffset] = input.pos();
		}

		void fillMapsInfo()
		{
			const Offset currentOffset = input.pos();
			while (skipToMap())
			{
				fillMap();
			}

			input.seek(currentOffset);
		}
	};

JsonParserStreamReader::JsonParserStreamReader(QIODevice& input)
	: m_data(new JsonParserStreamReaderData(input))
{

}

JsonParserStreamReader::JsonParserStreamReader(const JsonParserStreamReader& other)
	: m_data(other.m_data)
{

}

JsonParserStreamReader::ElementType JsonParserStreamReader::nextElementType()
{
	if (JsonParser::checkChar(m_data->input, '{'))
	{
		return Map;
	}

	if (JsonParser::checkChar(m_data->input, '['))
	{
		return List;
	}

	return End;
}

QVariant JsonParserStreamReader::readCompount()
{
	QVariant result = JsonParser::parseJson(d()->input);
	d()->skipComma();
	return result;
}

JsonParserStreamReaderData* JsonParserStreamReader::d()
{
	return m_data.get();
}


JsonStreamMapReader::JsonStreamMapReader(JsonParserStreamReader& stream)
	: JsonParserStreamReader(stream)	
	, m_nestedMapReader(false)
{	
	if (JsonParserStreamReader::nextElementType() != Map)
	{
		if (!d()->mapsStack.isEmpty())
		{
			m_nestedMapReader = true;
			m_mapPos = d()->mapsStack.top();
		}
		else
		{
			int line;
			int col;
			JsonParser::currentLineCol(d()->input, line, col);
			throw JsonParserException("Expected '{' at beginning of map", line, col);			
		}		
	}
	else
	{
		m_mapPos = d()->input.pos();
		if (!d()->maps.contains(m_mapPos))
		{
			int line;
			int col;
			JsonParser::currentLineCol(d()->input, line, col);
			throw JsonParserException("Expected '{' at beginning of map", line, col);	
		}
		d()->mapsStack.push(m_mapPos);
	}	

	m_endOfMap = d()->endOfMaps[m_mapPos];
}

JsonStreamMapReader::~JsonStreamMapReader()
{
	if (!m_nestedMapReader)
	{
		d()->input.seek(m_endOfMap);
		d()->skipComma();
		d()->mapsStack.pop();
	}	
}

QStringList JsonStreamMapReader::keys()
{
	return d()->maps[m_mapPos].keys();
}

bool JsonStreamMapReader::seekValue(const QString& key)
{
	quint64 pos = d()->maps[m_mapPos].value(key, 0);

	if (pos == 0)
	{
		return false;
	}	

	d()->input.seek(pos);

	return true;
}

JsonParserStreamReader& JsonStreamMapReader::value(const QString& key)
{
	if(!seekValue(key))
	{
		QString message = QString("Key ") % key % QString(" not found");
		//ERRLOG << "Key" << key << " not found";
		ASSERT(!"Key not found");
	}
	return *this;
}

QVariant JsonStreamMapReader::readValue(const QString& key)
{
	QVariant result = readCompount(key);

	ASSERT(result.type() != QMetaType::QVariantMap && result.type() != QMetaType::QVariantList);

	return result;
}

QVariant JsonStreamMapReader::readCompount(const QString& key)
{
	if (!seekValue(key))
	{
		return QVariant();
	}

	return JsonParser::parseJson(d()->input);	
}

QVariant JsonStreamMapReader::operator[](const QString& key)
{
	return readCompount(key);
}

JsonStreamMapReader::ElementType JsonStreamMapReader::nextElementType()
{
	ASSERT(!"not implemented: use valueType() instead");
	return End;
}

JsonStreamMapReader::ElementType JsonStreamMapReader::valueType(const QString& key)
{
	value(key);

	JsonStreamMapReader::ElementType result = JsonParserStreamReader::nextElementType();

	if (result == End)
	{
		return Value;
	}

	return result;
}

JsonStreamListReader::JsonStreamListReader(JsonParserStreamReader& stream)
	: JsonParserStreamReader(stream)
	, m_endOfList(d()->endOfLists.value(d()->input.pos(), 0))
{
	if (m_endOfList == 0 || !JsonParser::tryReadChar(d()->input, '['))
	{
		int line;
		int col;
		JsonParser::currentLineCol(d()->input, line, col);
		throw JsonParserException("Expected '[' at beginning of list", line, col);	
	}	

	JsonParser::skipJson(d()->input);	
}

JsonStreamListReader::~JsonStreamListReader()
{
	d()->input.seek(m_endOfList);
	d()->skipComma();
}

bool JsonStreamListReader::endOfList()
{
	return JsonParser::checkChar(d()->input, ']');	
}

JsonStreamMapReader::ElementType JsonStreamListReader::nextElementType()
{
	if (endOfList())
	{
		return End;
	}

	JsonStreamMapReader::ElementType result = JsonParserStreamReader::nextElementType();

	if (result == End)
	{
		return Value;
	}

	return result;
}

QVariant JsonStreamListReader::readValue()
{
	QVariant result = readCompount();

	ASSERT(result.type() != QMetaType::QVariantMap && result.type() != QMetaType::QVariantList);

	return result;
}

}
