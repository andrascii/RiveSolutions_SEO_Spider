#pragma once

namespace CrawlerEngine
{

class CustomUrl : public QUrl
{
public:
	using QUrl::QUrl;
	using QUrl::operator=;

	bool compareWith(const CustomUrl& url) const;
	const QString& canonizedUrlStr() const;

	bool operator==(const CustomUrl& url) const = delete;
	bool operator!=(const CustomUrl& url) const = delete;

private:
	mutable QString m_canonizedUrlStr;
};

}