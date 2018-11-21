#include "response_headers.h"

namespace
{

const QString s_mainHeader = "the main header";
const char* s_endHeaderName = ":";
const char s_endOfHeader[] = "\r\n";

}

namespace CrawlerEngine
{

void ResponseHeaders::addHeaderValue(const std::pair<QString, QString>& headerValue)
{
	m_responseHeaders.push_back(headerValue);
}

void ResponseHeaders::addHeaderValue(const QString& header, const QString& value)
{
	m_responseHeaders.push_back(std::make_pair(header.trimmed(), value.trimmed()));
}

bool ResponseHeaders::removeHeaderValues(const QString& header)
{
	auto it = std::remove_if(std::begin(m_responseHeaders), std::end(m_responseHeaders),
		[&header](const std::pair<QString, QString>& item) { return header == item.first; });

	const bool result = it != std::end(m_responseHeaders);

	m_responseHeaders.erase(it, std::end(m_responseHeaders));

	return result;
}

ResponseHeaders::ResponseHeaders(const QList<QNetworkReply::RawHeaderPair>& headerValues)
{
	addHeaderValues(headerValues);
}

void ResponseHeaders::buildFromByteArray(const QByteArray& responseData)
{
	int position = 0;

	while (true)
	{
		const int delimeterPosition = responseData.indexOf(s_endHeaderName, position);
		const int headerEndPosition = responseData.indexOf(s_endOfHeader, position);

		if (delimeterPosition == -1 || headerEndPosition == -1)
		{
			return;
		}

		if (delimeterPosition > headerEndPosition)
		{
			// here we have the first header
			addHeaderValue(s_mainHeader, responseData.mid(0, headerEndPosition));
			position += headerEndPosition + static_cast<int>(std::size(s_endOfHeader)) - 1;

			continue;
		}

		const QByteArray headerName = responseData.mid(position, delimeterPosition - position);
		const QByteArray headerValue = responseData.mid(delimeterPosition + 1, headerEndPosition - delimeterPosition + 1);

		position = headerEndPosition + static_cast<int>(std::size(s_endOfHeader)) - 1;

		addHeaderValue(headerName, headerValue);
	}
}

void ResponseHeaders::addHeaderValues(const QList<QNetworkReply::RawHeaderPair>& headerValues)
{
	for (const QNetworkReply::RawHeaderPair& rawHeaderPair : headerValues)
	{
		addHeaderValue(rawHeaderPair.first, rawHeaderPair.second);
	}
}

std::vector<QString> ResponseHeaders::valueOf(const QString& header) const
{
	std::vector<QString> values;

	for (const auto& [headerName, value] : m_responseHeaders)
	{
		if (!headerName.compare(header, Qt::CaseInsensitive))
		{
			values.push_back(value);
		}
	}

	return values;
}

QString ResponseHeaders::mainHeader() const noexcept
{
	std::vector<QString> value = valueOf(s_mainHeader);

	if(value.empty())
	{
		return QString();
	}

	return value[0];
}

QString ResponseHeaders::makeString() const
{
	QString serverResponse;

	const QString colon = QStringLiteral(": ");
	const QString lineEnd = QStringLiteral("\r\n");

	for (const auto& [headerName, value] : m_responseHeaders)
	{
		serverResponse = serverResponse % headerName % colon % value % lineEnd;
	}

	// finalize string
	serverResponse = serverResponse % lineEnd;

	serverResponse.squeeze();
	return serverResponse;
}

}