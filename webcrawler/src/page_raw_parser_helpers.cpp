#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

QUrl PageRawParserHelpers::resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl)
{
	QUrl result = relativeUrl;

	//
	// see: https://tools.ietf.org/html/rfc1808#section-4
	//

	assert(!baseUrl.isRelative() && "Base URL always MUST BE an absolute URL!!!");


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

}