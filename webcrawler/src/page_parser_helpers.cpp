#include "page_parser_helpers.h"

namespace WebCrawler
{

QUrl PageParserHelpers::resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl)
{
	QUrl result = relativeUrl;

	if (result.toDisplayString().startsWith("//"))
	{
		return QString("http:") + result.toDisplayString();
	}

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

	QString fixedPath = result.path();
	if (fixedPath.contains("/./"))
	{
		fixedPath = fixedPath.replace("/./", "/");
	}
	
	while (fixedPath.contains("/../"))
	{
		int doublePointsPos = fixedPath.indexOf("/../");
		int prevSlashPos = fixedPath.left(doublePointsPos).lastIndexOf("/");
		if (prevSlashPos == -1)
		{
			WARNINGLOG << "Can't resolve absolute path: " << fixedPath;
			break;
		}
		
		fixedPath = fixedPath.left(prevSlashPos + 1) + fixedPath.right(fixedPath.size() - doublePointsPos - 4);
	}

	result.setPath(fixedPath);

	return result;
}

std::vector<QUrl> PageParserHelpers::resolveUrlList(const QUrl& baseUrl, const std::vector<QUrl>& urlList) noexcept
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

	for (QUrl& url : result)
	{
		url.setFragment(QString());
	}

	return result;
}

bool PageParserHelpers::isUrlExternal(const QUrl& baseUrl, const QUrl& url) noexcept
{
	// TODO: improve
	QString baseUrlHost = baseUrl.host().toLower();
	QString urlHost = url.host().toLower();
	
	const bool isUrlInternal =  baseUrlHost == urlHost || 
		(urlHost.endsWith("www." + baseUrlHost)) ||
		(baseUrlHost.endsWith("www." + urlHost));

	// TODO: what if both urls are sub-domains?

	return !isUrlInternal;
}

bool PageParserHelpers::isHtmlContentType(const QString& contentType) noexcept
{
	return contentType.startsWith("text/html") ||
		contentType.startsWith("text/xhtml") ||
		contentType.startsWith("application/xhtml") ||
		contentType.isEmpty();
}

bool PageParserHelpers::isHttpOrHttpsScheme(const QString& urlStr) noexcept
{
	static QRegularExpression expr("^\\w{0,10}:");

	if (expr.match(urlStr).hasMatch() && !urlStr.startsWith("https://") && !urlStr.startsWith("http://"))
	{
		//DEBUGLOG << "Non-http link: " << urlStr;
		return false;
	}

	return true;

}

}