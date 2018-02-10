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

	inline bool compare(const Url& url) const
	{
		return urlStr() == url.urlStr();
	}

	QString urlStr() const
	{
		return toDisplayString();
	}
	
	inline bool operator==(const Url& url) const
	{
		return compare(url);
	}
	
	inline bool operator!=(const Url& url) const
	{
		return !compare(url);
	}

	const QString& canonizedUrlStr() const;

private:
	mutable QString m_canonizedUrlStr;
};

}