#pragma once

#include <memory>

namespace Common
{
	struct JsonParserStreamReaderData;

	class JsonParserStreamReader
	{
	public:
		enum ElementType
		{
			Map,
			List,
			Value,
			End
		};
	public:
		JsonParserStreamReader(QIODevice& input);
		JsonParserStreamReader(const JsonParserStreamReader& other);

		virtual ElementType nextElementType();		

		QVariant readCompount();

	protected:
		JsonParserStreamReaderData* d();

	private:
		std::shared_ptr<JsonParserStreamReaderData> m_data;
	};

	class JsonStreamMapReader : public JsonParserStreamReader
	{
	public:
		JsonStreamMapReader(JsonParserStreamReader& stream);
		~JsonStreamMapReader();

		QStringList keys();		

		JsonParserStreamReader& value(const QString& key);

		QVariant readValue(const QString& key);

		QVariant readCompount(const QString& key);

		//! the same as readCompound
		QVariant operator[](const QString& key);

	public:
		//! not implemented, use valueType instead
		virtual ElementType nextElementType();

		ElementType valueType(const QString& key);

	private:
		bool seekValue(const QString& key);

	private:
		quint64 m_mapPos;
		quint64 m_endOfMap;
		bool m_nestedMapReader;
	};

	class JsonStreamListReader : public JsonParserStreamReader
	{
	public:
		JsonStreamListReader(JsonParserStreamReader& stream);
		~JsonStreamListReader();

		QVariant readValue();

		bool endOfList();

	public:
		virtual ElementType nextElementType();

	private:
		quint64 m_endOfList;
	};
}
