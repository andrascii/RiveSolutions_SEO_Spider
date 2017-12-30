#pragma once

namespace CrawlerEngine
{

class Url : public QUrl
{
public:
	using QUrl::QUrl;
	using QUrl::operator=;

	Url() = default;
	Url(const QUrl& url);

	bool compare(const Url& url) const;
	const QString& canonizedUrlStr() const;

	bool operator==(const Url& url) const = delete;
	bool operator!=(const Url& url) const = delete;

private:
	mutable QString m_canonizedUrlStr;
};

}