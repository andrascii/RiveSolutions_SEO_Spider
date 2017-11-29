#ifndef _JSONPARSER_H_
#define _JSONPARSER_H_

//#include "shellapidefs.h"

namespace Common
{
class JsonParserException : public std::runtime_error
{
public:
	JsonParserException(const std::string& message, int line, int col) 
		: std::runtime_error(message), m_line(line), m_col(col)
	{
	}
	int line() const { return m_line; }
	int col() const { return m_col; }
private:
	int m_line;
	int m_col;
};

//! Qt-based simple JSON initialy taken from <a href='http://stellarium.svn.sourceforge.net/'>Stellarium</a>.
//! JSON is JavaScript Object Notation. See http://www.json.org/
/*! <p>The mapping with Qt types is done as following:
@verbatim
JSON            Qt
----          -------
null          QVariant::Invalid
object        QVariantMap (QVariant::Map)
array         QVariantList (QVariant::List)
boolean       QVariant::Bool
string        QVariant::String
number        QVariant::Int or QVariant::Double
@endverbatim */

class JsonParser
{
public:
	//! Parse the given input stream
	//! throws JsonParserException
	QVariant parse(QIODevice& input) const;

	//! overloaded method
	//! throws JsonParserException
	QVariant parse(const QByteArray& jsonString) const;

	QVariant parseNoThrow(const QByteArray& jsonString) const;

	//! Serialize the passed QVariant as JSON into the output QIODevice
	void write(const QVariant& jsonObject, QIODevice& output) const;

	//! Serialize the passed QVariant as JSON into the output QIODevice
	void writeNoIndentation(const QVariant& jsonObject, QIODevice& output) const;

	//! overloaded method
	QByteArray writeNoIndentation(const QVariant& json) const;

public:
	static void writeJson(const QVariant& v, QIODevice& output, int indentLevel, bool useIndentation);

	static void startList(QIODevice &output);	

	static int prepareToWriteListItem(QIODevice& output, const QVariant& v, const bool notFirstValue, 
		int indentLevel, bool useIndentation);

	static void endList(QIODevice &output, int indentLevel, bool useIndentation);

	static void startMap(QIODevice& output, bool useIndentation);	

	static int prepareToWriteMapValue(QIODevice &output, const QVariant& v, int indentLevel, bool useIndentation);

	static void prepareMapValueKey(QIODevice &output, const QString& key, 
		const bool notFirstValue, int indentLevel, bool useIndentation);

	static void endMap(QIODevice& output, int indentLevel, bool useIndentation);		

	static bool tryReadChar(QIODevice& input, char c);
	static bool checkChar(QIODevice& input, char c);

	static QVariant parseJson(QIODevice& input);	

	static QString readMapKey(QIODevice &input);

	static void skipJson(QIODevice& input);

	static void currentLineCol(QIODevice &input, int& line, int& col);

private:
	static void writeMapValue(QIODevice &output, const QString& key, const QVariant& v, 
		const bool notFirstValue, int indentLevel, bool useIndentation);

	static void writeListItem(QIODevice &output, const QVariant &v, const bool notFirstValue, 
		int indentLevel, bool useIndentation);
	
	static QString readQuotedString(QIODevice& input);
	static QString readString(QIODevice& input);
};
}


#endif // _SONPARSER_HPP_
