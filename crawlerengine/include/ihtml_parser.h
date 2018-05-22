#pragma once

namespace CrawlerEngine
{

class ResponseHeaders;
struct LinkInfo;

class IHtmlParser
{
public:
	virtual ~IHtmlParser() = default;

	virtual QByteArray identifyHtmlPageContentType() const = 0;
	virtual QByteArray decodeHtmlPage(const ResponseHeaders& headers) const = 0;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const = 0;
};

}