#include "response_headers.h"

namespace CrawlerEngine
{

void ResponseHeaders::addHeaderValue(const std::pair<QString, QString>& headerValue)
{
	m_responseHeaders.push_back(headerValue);
}

void ResponseHeaders::addHeaderValue(const QString& header, const QString& value)
{
	m_responseHeaders.push_back(std::make_pair(header, value));
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

void ResponseHeaders::addHeaderValues(const QList<QNetworkReply::RawHeaderPair>& headerValues)
{
	for (const QNetworkReply::RawHeaderPair& rawHeaderPair : headerValues)
	{
		addHeaderValue(rawHeaderPair.first, rawHeaderPair.second);
	}
}

std::vector<QString> ResponseHeaders::valueOf(const QString& header) const noexcept
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

QString ResponseHeaders::makeString() const
{
	QString serverResponse;

	for (const auto& [headerName, value] : m_responseHeaders)
	{
		serverResponse += headerName + QStringLiteral(": ") + value + QStringLiteral("\r\n");
	}

	// finalize string
	serverResponse += QStringLiteral("\r\n");

	return serverResponse;
}

}