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

void ResponseHeaders::addHeaderValues(const QList<QNetworkReply::RawHeaderPair>& headerValues)
{
	m_responseHeaders.reserve(m_responseHeaders.size() + headerValues.size());

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
		serverResponse += headerName + QStringLiteral(": ") + value + QStringLiteral("\n\r");
	}

	// finalize string
	serverResponse += QStringLiteral("\n\r");

	return serverResponse;
}

}