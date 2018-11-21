#pragma once

namespace CrawlerEngine
{

class ResponseHeaders final
{
public:
	ResponseHeaders() = default;
	ResponseHeaders(const QList<QNetworkReply::RawHeaderPair>& headerValues);

	void buildFromByteArray(const QByteArray& responseData);
	void addHeaderValues(const QList<QNetworkReply::RawHeaderPair>& headerValues);
	void addHeaderValue(const std::pair<QString, QString>& headerValue);
	void addHeaderValue(const QString& header, const QString& value);
	bool removeHeaderValues(const QString& header);
	std::vector<QString> valueOf(const QString& header) const;
	QString mainHeader() const noexcept;
	QString makeString() const;

private:
	std::vector<std::pair<QString, QString>> m_responseHeaders;
};

}