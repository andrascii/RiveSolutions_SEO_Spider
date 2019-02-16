#pragma once

namespace SeoSpiderServiceApi
{

#define DUMP(arg) #arg, arg
#define MAKE_DUMP_DATA(...) SeoSpiderServiceApi::MakeDumpData(__VA_ARGS__).data()

class MakeDumpData
{
public:
	template <typename... Args>
	MakeDumpData(Args&&... args)
	{
		static_assert(sizeof...(args) % 2 == 0, "The size of argument pack must be even number");

		QByteArray buffer;
		QTextStream stream(&buffer);
		fillStreamHelper(stream, static_cast<std::decay_t<Args>>(std::forward<Args>(args))...);
		stream.flush();
		m_data = buffer.toStdString();
	}

	std::string data() const
	{
		return m_data;
	}

private:
	// TypeConverter is used to support variables whose type isn't compatible with QTextStream.
	// For example QTextStream cannot write std::string type.
	// Therefore we convert variables like that to QByteArray and so on...
	template <typename T>
	struct TypeConverter
	{
		TypeConverter(T&& object)
			: result(std::move(object))
		{
		}

		T result;
	};

	template <>
	struct TypeConverter<std::string>
	{
		TypeConverter(const std::string& object)
			: result(object.data())
		{
		}

		QByteArray result;
	};

	template <typename Head, typename... Args>
	void fillStreamHelper(QTextStream& stream, Head&& head, Args&&... args)
	{
		TypeConverter converter(static_cast<std::decay_t<Head>>(head));

		if constexpr (sizeof...(args) % 2 == 1)
		{
			stream << converter.result << "=";
		}
		else
		{
#ifdef QT_DEBUG
			stream << converter.result << "\n";
#else
			stream << converter.result << ", ";
#endif
		}

		fillStreamHelper(stream, std::forward<Args>(args)...);
	}

	template <typename Last>
	void fillStreamHelper(QTextStream& stream, Last&& last)
	{
		TypeConverter converter(static_cast<std::decay_t<Last>>(last));
		stream << converter.result;
	}

private:
	std::string m_data;
};

}