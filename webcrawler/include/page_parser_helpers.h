#pragma once

#include "parsed_page.h"

namespace WebCrawler
{

class PageParserHelpers
{
public:
	static QUrl resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl);
	static std::vector<QUrl> resolveUrlList(const QUrl& baseUrl, const std::vector<QUrl>& urlList) noexcept;
	static std::vector<LinkInfo> resolveUrlList(const QUrl& baseUrl, const std::vector<LinkInfo>& linkList) noexcept;
	static QUrl resolveUrl(const QUrl& baseUrl, const QUrl& url) noexcept;

	static bool isUrlExternal(const QUrl& baseUrl, const QUrl& url) noexcept;
	static bool isHtmlContentType(const QString& contentType) noexcept;
	static bool isHttpOrHttpsScheme(const QUrl& url) noexcept;
	static bool isSubdomain(const QUrl& baseUrl, const QUrl& url);
};

}