#pragma once

namespace CrawlerEngine
{

class CustomUrl : public QUrl
{
public:
	using QUrl::QUrl;
	using QUrl::operator=;

	bool compareWith(const CustomUrl& url) const;
	
private:
	const QString& canonizedUrlStr() const;

private:
	mutable QString m_canonizedUrlStr;
};

}