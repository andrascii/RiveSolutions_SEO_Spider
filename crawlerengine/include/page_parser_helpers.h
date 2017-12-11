#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

class PageParserHelpers
{
public:
	static QUrl resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl);
	static std::vector<QUrl> resolveUrlList(const QUrl& baseUrl, const std::vector<QUrl>& urlList) noexcept;
	static std::vector<LinkInfo> resolveUrlList(const QUrl& baseUrl, const std::vector<LinkInfo>& linkList) noexcept;
	static QUrl resolveUrl(const QUrl& baseUrl, const QUrl& url) noexcept;

	static QUrl removeUrlLastSlashIfExists(const QUrl& url);
	static bool checkUrlIdentity(const QUrl& lhs, const QUrl& rhs);

	static bool isUrlExternal(const QUrl& baseUrl, const QUrl& url) noexcept;
	static bool isHtmlOrPlainContentType(const QString& contentType) noexcept;
	static bool isHttpOrHttpsScheme(const QUrl& url) noexcept;
	static bool isSubdomain(const QUrl& baseUrl, const QUrl& url);
};

}