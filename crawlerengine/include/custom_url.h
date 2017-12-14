#pragma once

namespace CrawlerEngine
{

class CustomUrl : public QUrl
{
public:
	using QUrl::QUrl;
	using QUrl::operator=;

	bool compareWith(const QUrl& url) const;
};

}