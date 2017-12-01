#include "json_parser.h"

namespace Common
{
	static QByteArray s_eol("\n");

	void JsonParser::skipJson(QIODevice& input)
	{
		// There is a weakness in this code -- it will cause any standalone '/' to be absorbed.
		char c;
		while (input.getChar(&c))
		{
			if (QChar(QLatin1Char(c)).isSpace() || QLatin1Char(c) == QLatin1Char('\n'))
			{
				continue;
			}
			else
			{
				if (c!='/')
				{
					input.ungetChar(c);
					return;
				}

				if (!input.getChar(&c))
					return;

				if (c=='/')
				{
					input.readLine();
				}
				else
				{
					// We have a problem, we removed an '/'..
					qWarning() << "I removed a '/' while parsing JSON file.";
					input.ungetChar(c);
					return;
				}
			}
		}
	}

	bool JsonParser::tryReadChar(QIODevice& input, char c)
	{
		char r;
		if (!input.getChar(&r))
			return false;

		if (r == c)
			return true;

		input.ungetChar(r);
		return false;
	}

	bool JsonParser::checkChar(QIODevice& input, char c)
	{
		char r;
		if (!input.getChar(&r))
			return false;

		input.ungetChar(r);

		return (r == c);
	}


	QString JsonParser::readQuotedString(QIODevice& input)
	{
		QByteArray name;
		char c;
		input.getChar(&c);
		if (c!='\"')
		{
			int line;
			int col;
			currentLineCol(input, line, col);
			throw JsonParserException("Expected '\"' at beginning of string", line, col);
		}
		for (;;)
		{
			if (input.atEnd())
			{
				//throw std::runtime_error(qPrintable(QString(QLatin1String("End of file before end of string: ")+name)));
				const QByteArray s = "End of file before end of string: " + name;
				int line;
				int col;
				currentLineCol(input, line, col);
				throw JsonParserException(s.constData(), line, col);
			}
			input.getChar(&c);
			bool isSpecialChar = false;
			if (c=='\\')
			{
				isSpecialChar = true;
				input.getChar(&c);
				if (c=='b') c='\b';
				if (c=='f') c='\f';
				if (c=='n') c='\n';
				if (c=='r') c='\r';
				if (c=='t') c='\t';
				if (c=='u')
				{
					QByteArray value;
					for (int i = 0; i < 4; ++i)
					{
						input.getChar(&c);
						value += c;
					}

					bool ok;
					int code = value.toInt(&ok, 16);
					if (!ok)
					{
						int line;
						int col;
						currentLineCol(input, line, col);
						throw JsonParserException("Wrong value in \\uxxxx part", line, col);
					}

					const QChar ch(code);
					name += QString(ch).toUtf8();
					continue;
				}
			}
			if (!isSpecialChar && c=='\"')
				return QString::fromUtf8(name);
			name+=c;
		}
		Q_ASSERT(0);
		return QLatin1String("");
	}

	QString JsonParser::readString(QIODevice& input)
	{
		QByteArray name;
		char c;
		input.getChar(&c);
		if (c=='\"')
		{
			input.ungetChar(c);
			return readQuotedString(input);
		}
		name+=c;
		for (;;)
		{
			if (input.atEnd())
			{
				const QByteArray s = "End of file before end of string: " + name;
				int line;
				int col;
				currentLineCol(input, line, col);
				throw JsonParserException(s.constData(), line, col);
			}
			input.getChar(&c);
			if (!QChar(QLatin1Char(c)).isLetterOrNumber())
			{
				input.ungetChar(c);
				return QString::fromUtf8(name);
			}
			name+=c;
		}
		Q_ASSERT(0);
		return QLatin1String("");
	}

	QVariant readOther(QIODevice& input)
	{
		QByteArray str;
		char c;
		while (input.getChar(&c))
		{
			if (QChar(QLatin1Char(c)).isSpace() || c==']' || c=='}' || c==',')
			{
				input.ungetChar(c);
				break;
			}
			str+=c;
		}
		QTextStream ts(&str);
		QString s;
		ts >> s;
		if (s==QLatin1String("true"))
			return QVariant(true);
		if (s==QLatin1String("false"))
			return QVariant(false);
		if (s==QLatin1String("null"))
			return QVariant();
		QVariant v(s);
		bool ok=false;
		int i = v.toInt(&ok);
		if (ok) return i;
		double d = v.toDouble(&ok);
		if (ok)
			return d;
		return v;
	}


	QVariant JsonParser::parseJson(QIODevice& input)
	{
		skipJson(input);

		if (tryReadChar(input, '{'))
		{
			// We've got an object (a tuple)
			QVariantMap map;
			skipJson(input);
			if (tryReadChar(input, '}'))
				return map;
			for (;;)
			{
				const QString key = readMapKey(input);
				
				map.insert(key, parseJson(input));
				skipJson(input);

				if (!tryReadChar(input, ','))
					break;
			}

			skipJson(input);

			if (!tryReadChar(input, '}'))
			{
				int line;
				int col;
				currentLineCol(input, line, col);
				throw JsonParserException("Expected '}' to close an object", line, col);
			}

			return map;
		}
		else if (tryReadChar(input, '['))
		{
			// We've got an array (a vector)
			QVariantList list;
			skipJson(input);
			if (tryReadChar(input, ']'))
				return list;

			for (;;)
			{
				list.append(parseJson(input));
				skipJson(input);
				if (!tryReadChar(input, ','))
					break;
			}

			skipJson(input);

			if (!tryReadChar(input, ']'))
			{
				int line;
				int col;
				currentLineCol(input, line, col);
				throw JsonParserException("Expected ']' to close an array", line, col);
			}

			return list;
		}
		else if (tryReadChar(input, '\"'))
		{
			// We've got a string
			input.ungetChar('\"');
			return readQuotedString(input);
		}
		return readOther(input);
	}

	bool startNewLine(QVariant::Type type)
	{
		switch (type)
		{
		case QVariant::Bool:
		case QVariant::Invalid:
		case QVariant::ByteArray:
		case QVariant::String:
		case QVariant::ULongLong:
		case QVariant::Int:
		case QVariant::Double:
			return false;
		}
		return true;
	}

	void JsonParser::prepareMapValueKey(QIODevice &output, const QString& key,
		const bool notFirstValue, int indentLevel, bool useIndentation)
	{
		const QByteArray prepend = useIndentation ? QByteArray(indentLevel, '\t') : QByteArray();
		const QByteArray newline = useIndentation ? QByteArray("\n") : QByteArray();

		if (notFirstValue)
		{
			output.putChar(',');				
			output.write(newline);
		}

		output.write(prepend);
		output.write("\"");
		output.write(key.toUtf8());
		output.write("\": ");
	}

	int JsonParser::prepareToWriteMapValue(QIODevice &output, const QVariant& v, int indentLevel, bool useIndentation)
	{
		const QVariant::Type vType = v.type();

		int itemIndent = 0;
		// Break line if we start an JSON Object for nice looking
		if (startNewLine(vType))
		{
			output.write(useIndentation ? s_eol : QByteArray());
			itemIndent = indentLevel;
		}

		return itemIndent;
	}

	void JsonParser::writeMapValue(QIODevice &output, const QString& key, const QVariant& v, 
		const bool notFirstValue, int indentLevel, bool useIndentation)
	{
		prepareMapValueKey(output, key, notFirstValue, indentLevel, useIndentation);

		const int itemIndent = prepareToWriteMapValue(output, v, indentLevel, useIndentation);

		writeJson(v, output, itemIndent, useIndentation);
	}

	void JsonParser::startMap(QIODevice& output, bool useIndentation)
	{
		output.write("{");
		output.write(useIndentation ? s_eol : QByteArray());
	}

	void JsonParser::endMap(QIODevice& output, int indentLevel, bool useIndentation)
	{
		output.write(useIndentation ? s_eol : QByteArray());
		output.write(useIndentation ? QByteArray(indentLevel, '\t') : QByteArray());
		output.write("}");
	}

	void JsonParser::startList(QIODevice &output)
	{
		output.write("[");
	}

	void JsonParser::endList(QIODevice &output, int indentLevel, bool useIndentation)
	{
		output.write(useIndentation ? s_eol : QByteArray());
		output.write(useIndentation ? QByteArray(indentLevel, '\t') : QByteArray());
		output.write("]");
	}

	int JsonParser::prepareToWriteListItem(QIODevice& output, const QVariant& v, const bool notFirstValue, 
		int indentLevel, bool useIndentation)
	{
		int itemIndent = 0;
		if (notFirstValue)
		{
			output.write(", ");				
		}
		if (!notFirstValue || startNewLine(v.type()))
		{
			output.write(useIndentation ? s_eol : QByteArray());						
			itemIndent = indentLevel;
		}
		return itemIndent;
	}

	void JsonParser::writeListItem(QIODevice& output, const QVariant& v, const bool notFirstValue, 
		int indentLevel, bool useIndentation)
	{
		const int itemIndent = prepareToWriteListItem(output, v, notFirstValue, indentLevel, useIndentation);

		writeJson(v, output, itemIndent, useIndentation);
	}

	void JsonParser::writeJson(const QVariant& v, QIODevice& output, int indentLevel, bool useIndentation)
	{
		const QByteArray prepend = useIndentation ? QByteArray(indentLevel, '\t') : QByteArray();
		const QByteArray newline = useIndentation ? s_eol : QByteArray();
		output.write(prepend);
		switch (v.type())
		{
		case QVariant::Bool:
			output.write(v.toBool() == true ? "true" : "false");
			break;
		case QVariant::Invalid:
			output.write("null");
			break;
		case QVariant::ByteArray:
		case QVariant::String:
		
			{
				QByteArray s;
				if (QVariant::String == v.type())
				{
					s = v.toString().toUtf8();
				}
				else
				{
					s = v.toByteArray();
				}
				s.replace('\\', "\\\\");
				s.replace('\"', "\\\"");
				s.replace('\b', "\\b");
				s.replace('\n', "\\n");
				s.replace('\f', "\\f");
				s.replace('\r', "\\r");
				s.replace('\t', "\\t");
				output.write("\"");
				output.write(s);
				output.write("\"");
				break;
			}
		case QVariant::ULongLong:
		case QVariant::Int:
			output.write(v.toString().toUtf8());
			break;
		case QVariant::Double:
			{
				const QString s = v.toString();
				output.write(s.toUtf8());
				if (-1 == s.indexOf(QLatin1Char('.')) && -1 == s.indexOf(QLatin1Char('e')) && -1 == s.indexOf(QLatin1Char('E')))
				{
					// we need to distinguish double/int when read afterward
					output.write(".0");
				}				
				break;
			}
		case QVariant::List:
			{
				startList(output);				

				const QVariantList l = v.toList();
				for (int i=0; i<l.size(); ++i)
				{
					const QVariant& v = l.at(i);

					const bool notFirstValue = i != 0;;
					writeListItem(output, v, notFirstValue, indentLevel + 1, useIndentation);
				}

				endList(output, indentLevel, useIndentation);

				break;
			}
		case QVariant::Map:
			{
				startMap(output, useIndentation);

				QVariantMap m = v.toMap();

				int j = 0;			
				for (QVariantMap::ConstIterator i = m.constBegin() ; i != m.constEnd() ; ++i)
				{
					const bool notFirstValue = j != 0;

					writeMapValue(output, i.key(), i.value(), notFirstValue, indentLevel + 1, useIndentation);

					++j;
				}

				endMap(output, indentLevel, useIndentation);
				break;
			}
		default:
			{
				qWarning() << "Cannot serialize QVariant of type " << v.typeName() << " in JSON";
				ASSERT(!"Unable to serialize provided QVariant: unsupported type");
			}
		}
	}

	QVariant JsonParser::parse(QIODevice& _input) const
	{
		return parseJson(_input);
	}

	QVariant JsonParser::parse(const QByteArray& jsonString) const
	{
		QBuffer buffer;
		buffer.setData(jsonString);
		buffer.open(QIODevice::ReadOnly);
		return parse(buffer);
	}

	void JsonParser::write(const QVariant& jsonObject, QIODevice& output) const
	{
		return writeJson(jsonObject, output, 0, true);
	}

	void JsonParser::writeNoIndentation(const QVariant& jsonObject, QIODevice& output) const
	{
		return writeJson(jsonObject, output, 0, false);
	}

	QByteArray JsonParser::writeNoIndentation(const QVariant& json) const
	{
		QBuffer buffer;
		buffer.open(QIODevice::WriteOnly);
		writeNoIndentation(json, buffer);
		buffer.close();
		return buffer.data();
	}

	QVariant JsonParser::parseNoThrow(const QByteArray& jsonString) const
	{
		try
		{
			return parse(jsonString);
		}
		catch(const JsonParserException& exception)
		{
			Q_UNUSED(exception);
			//WARNLOG << exception.what();

			return QVariant();
		}
	}

	QString JsonParser::readMapKey(QIODevice &input)
	{
		skipJson(input);
		const QString key = readString(input);
		skipJson(input);
		if (!tryReadChar(input, ':'))
		{
			//throw std::runtime_error(qPrintable(QString("Expected ':' after a member name: ")+key));
			const QByteArray msg = "Expected ':' after a member name: " + key.toLatin1();
			int line;
			int col;
			currentLineCol(input, line, col);
			throw JsonParserException(msg.constData(), line, col);
		}
		skipJson(input);
		return key;
	}

	void JsonParser::currentLineCol(QIODevice &input, int& line, int& col)
	{
		qint64 currentPos = input.pos();
		ASSERT(!input.isSequential());
		input.reset();
		line = 0;
		
		while (input.pos() < currentPos)
		{
			const int count = input.readLine().length();
			if (currentPos > count)
			{
				currentPos -= count;
			}
			line++;
		}

		col = (int)currentPos;
	}

}

