#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

QUrl PageRawParserHelpers::resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl)
{
	QUrl result = relativeUrl;

	//
	// see: https://tools.ietf.org/html/rfc1808#section-4
	//

	DEBUG_ASSERT(!baseUrl.isRelative() && "Base URL always MUST BE an absolute URL!!!");


	if (!result.isRelative())
	{
		DEBUGLOG << "Passed non-relative url:" << result.toDisplayString();

		return result;
	}

	result.setScheme(baseUrl.scheme());
	result.setHost(baseUrl.host());

	QString pathWithoutFile = baseUrl.path();
	int lastSlashIndex = pathWithoutFile.lastIndexOf("/");

	if (result.path().startsWith("/"))
	{
		//
		// Just exit from function because we already make it absolute
		//
		return result;
	}
	else
	{
		//
		// Make path starts with slash for ensure valid QUrl behavior
		//
		result.setPath("/" + result.path());
	}

	if (result.path().isEmpty())
	{
		result.setPath(pathWithoutFile.left(lastSlashIndex + 1));
	}
	else
	{
		result.setPath(pathWithoutFile.left(lastSlashIndex) + result.path());
	}

	return result;
}

std::vector<QUrl> PageRawParserHelpers::resolveUrlList(const QUrl& baseUrl, const std::vector<QUrl>& urlList) noexcept
{
	std::vector<QUrl> result;
	for (const QUrl& url : urlList)
	{
		if (!url.isRelative())
		{
			result.push_back(url);
			continue;
		}

		result.push_back(resolveRelativeUrl(url, baseUrl));
	}

	return result;
}

bool PageRawParserHelpers::isUrlExternal(const QUrl& baseUrl, const QUrl& url) noexcept
{
	// TODO: improve
	QString baseUrlHost = baseUrl.host();
	QString urlHost = url.host();
	
	const bool isUrlInternal =  baseUrlHost == urlHost || 
		(urlHost.endsWith("." + baseUrlHost)) ||
		(baseUrlHost.endsWith("." + urlHost));

	// TODO: what if both urls are sub-domains?

	return !isUrlInternal;
}

bool PageRawParserHelpers::isHtmlContentType(const QString& contentType) noexcept
{
	return contentType.startsWith("text/html") ||
		contentType.startsWith("text/xhtml") ||
		contentType.startsWith("application/xhtml") ||
		contentType.isEmpty();
}

}