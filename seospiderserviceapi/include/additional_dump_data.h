#pragma once

namespace SeoSpiderServiceApi
{

#define DUMP(arg) #arg, arg
#define ADDITIONAL_DUMP_DATA(...) SeoSpiderServiceApi::createAdditionalDumpDataString(__VA_ARGS__)

class Dumper
{
public:
	template <typename... Args>
	Dumper(Args&&... args)
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
		if constexpr (sizeof...(args) % 2 == 1)
		{
			TypeConverter converter(static_cast<std::decay_t<Head>>(head));
			stream << converter.result << "=";
		}
		else
		{
			TypeConverter converter(static_cast<std::decay_t<Head>>(head));

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

template <typename... Args>
std::string createAdditionalDumpDataString(Args&&... args)
{
	return Dumper(std::forward<Args>(args)...).data();
}

}