#pragma once
#include <memory>

namespace Common
{
	struct JsonParserStreamWriterData;

	class JsonParserStreamWriter
	{
	public:
		JsonParserStreamWriter(QIODevice& output, bool noIndentation = false);		

		void writeValue(const QVariant& v);		

		void writeMapValue(const QString& key, const QVariant& v);
		
		void beginMap();
		void endMap();

		void beginMapValue(const QString& key);
		void endMapValue();		

		void beginList();
		void endList();		

		void writeCompound(const QVariant& v);

	protected:
		JsonParserStreamWriter(const JsonParserStreamWriter& stream);

		JsonParserStreamWriterData* d() const;

	private:
		bool increaseLevelItemsCount();
		int prepareToWrite(const QVariant& v);
		void insertTabs(int count);		

	private:
		std::shared_ptr<JsonParserStreamWriterData> m_data;
	};

	class JsonStreamMapElement : public JsonParserStreamWriter
	{
	public:
		JsonStreamMapElement(JsonParserStreamWriter& stream);	
		~JsonStreamMapElement();		

	private:
		//! special case to support weird case when base class appends some values to the map, created by a child class
		bool m_continuePrevMap;
	};

	class JsonStreamMapValue : public JsonParserStreamWriter
	{
	public:
		JsonStreamMapValue(JsonStreamMapElement& stream, const QString& key);
		//JsonStreamMapValue(const JsonStreamMapValue& other);
		~JsonStreamMapValue();		
	};

	class JsonStreamListElement : public JsonParserStreamWriter
	{
	public:
		JsonStreamListElement(JsonParserStreamWriter& stream);
		~JsonStreamListElement();
	};		
}
