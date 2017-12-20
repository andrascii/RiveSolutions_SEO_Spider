#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

class PageParserHelpers
{
public:
	static CustomUrl resolveRelativeUrl(const CustomUrl& relativeUrl, const CustomUrl& baseUrl);
	static std::vector<CustomUrl> resolveUrlList(const CustomUrl& baseUrl, const std::vector<CustomUrl>& urlList) noexcept;
	static std::vector<LinkInfo> resolveUrlList(const CustomUrl& baseUrl, const std::vector<LinkInfo>& linkList) noexcept;
	static CustomUrl resolveUrl(const CustomUrl& baseUrl, const CustomUrl& url) noexcept;
	static bool checkUrlIdentity(const CustomUrl& lhs, const CustomUrl& rhs);

	static bool isUrlExternal(const CustomUrl& baseUrl, const CustomUrl& url) noexcept;
	static bool isHtmlOrPlainContentType(const QString& contentType) noexcept;
	static bool isHttpOrHttpsScheme(const CustomUrl& url) noexcept;
	static bool isSubdomain(const CustomUrl& baseUrl, const CustomUrl& url);
};

}