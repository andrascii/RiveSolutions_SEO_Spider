#pragma once

namespace WebCrawler
{

class PageRawParserHelpers
{
public:
	static QUrl resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl);
	static std::vector<QUrl> resolveUrlList(const QUrl& baseUrl, const std::vector<QUrl>& urlList) noexcept;
	static bool isUrlExternal(const QUrl& baseUrl, const QUrl& url) noexcept;
	static bool isHtmlContentType(const QString& contentType) noexcept;
};

}