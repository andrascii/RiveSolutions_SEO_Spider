#pragma once

namespace CrawlerEngine
{

class ResponseHeaders
{
public:
	void addHeaderValue(const std::pair<QString, QString>& headerValue);
	void addHeaderValue(const QString& header, const QString& value);

	std::vector<QString> valueOf(const QString& header) const noexcept;

	QString makeString() const;

private:
	std::vector<std::pair<QString, QString>> m_responseHeaders;
};

}