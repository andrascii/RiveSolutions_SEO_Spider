#include "stdafx.h"
#include "page_parser_helpers.h"

namespace CrawlerEngine
{

Url PageParserHelpers::resolveRelativeUrl(const Url& relativeUrl, const Url& baseUrl)
{
	return baseUrl.resolved(relativeUrl);

	Url result = relativeUrl;

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
		// Make path starts with slash for ensure valid CustomUrl behavior
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
			WARNLOG << "Can't resolve absolute path: " << fixedPath;
			break;
		}

		fixedPath = fixedPath.left(prevSlashPos + 1) + fixedPath.right(fixedPath.size() - doublePointsPos - 4);
	}

	result.setPath(fixedPath);

	return result;
}

void PageParserHelpers::resolveUrlList(const Url& baseUrl, std::vector<Url>& urlList) noexcept
{
	for (Url& url : urlList)
	{
		url = resolveUrl(baseUrl, url);
	}

}

void PageParserHelpers::resolveUrlList(const Url& baseUrl, std::vector<LinkInfo>& linkList) noexcept
{
	for (LinkInfo& link : linkList)
	{
		link.url = resolveUrl(baseUrl, link.url);
	}
}

Url PageParserHelpers::resolveUrl(const Url& baseUrl, const Url& url) noexcept
{
	Url result = url;

	if (result.isRelative())
	{
		result = resolveRelativeUrl(result, baseUrl);
	}

	result.setFragment(QString());

	return result;
}

bool PageParserHelpers::checkUrlIdentity(const Url& lhs, const Url& rhs)
{
	QString&& lhsString = lhs.toDisplayString();
	QString&& rhsString = rhs.toDisplayString();

	if (lhsString.endsWith("/"))
	{
		lhsString = lhsString.left(lhsString.size() - 1);
	}

	if (rhsString.endsWith("/"))
	{
		rhsString = rhsString.left(rhsString.size() - 1);
	}

	return lhsString == rhsString;
}

bool PageParserHelpers::isUrlExternal(const Url& baseUrl, const Url& url, bool allowSubDomains) noexcept
{
	// TODO: improve
	QString baseUrlHost = baseUrl.host().toLower();
	QString urlHost = url.host().toLower();

	const bool isUrlInternal =  baseUrlHost == urlHost ||
		(urlHost.endsWith("www." + baseUrlHost)) ||
		(baseUrlHost.endsWith("www." + urlHost));

	if (!isUrlInternal && allowSubDomains)
	{
		return !isSubdomain(baseUrl, url);
	}

	return !isUrlInternal;
}

bool PageParserHelpers::isUrlInsideBaseUrlFolder(const Url& baseUrl, const Url& url) noexcept
{
	if (PageParserHelpers::isUrlExternal(baseUrl, url, false))
	{
		return false;
	}

	const QString baseUrlPath = baseUrl.path().toLower();

	if (baseUrlPath.isEmpty() || baseUrlPath == QString("/"))
	{
		return true;
	}

	const QString urlPath = url.path().toLower();

	if (baseUrlPath.size() > urlPath.size())
	{
		return false;
	}

	if (baseUrlPath.size() == urlPath.size())
	{
		return baseUrlPath == urlPath;
	}

	const QString folder =
		baseUrlPath.endsWith(QChar('/')) ?
		baseUrlPath :
		baseUrlPath + QChar('/');

	return urlPath.startsWith(folder);
}

bool PageParserHelpers::isHtmlOrPlainContentType(const QString& contentType) noexcept
{
	return contentType.startsWith("text/html") ||
		contentType.startsWith("text/xhtml") ||
		contentType.startsWith("text/xml") || // why xml document treated as a html content?
		contentType.startsWith("application/xhtml") ||
		contentType.startsWith("text/plain") ||
		contentType.isEmpty();
}

bool PageParserHelpers::isHttpOrHttpsScheme(const Url& url) noexcept
{
	return url.isRelative() || url.scheme() == QString("http") || url.scheme() == QString("https");
}


bool PageParserHelpers::isSubdomain(const Url& baseUrl, const Url& url)
{
	if (!isHttpOrHttpsScheme(baseUrl) || !isHttpOrHttpsScheme(url))
	{
		return false;
	}

	if (baseUrl.isRelative() || url.isRelative() || baseUrl.compare(url))
	{
		return false;
	}

	Url baseUrlWithoutScheme = baseUrl;
	Url targetUrlWithoutScheme = url;

	baseUrlWithoutScheme.setScheme(QString::null);
	targetUrlWithoutScheme.setScheme(QString::null);

	if (baseUrlWithoutScheme.host() == targetUrlWithoutScheme.host())
	{
		return false;
	}

	return baseUrlWithoutScheme.host().endsWith(targetUrlWithoutScheme.host()) ||
		targetUrlWithoutScheme.host().endsWith(baseUrlWithoutScheme.host());
}

}