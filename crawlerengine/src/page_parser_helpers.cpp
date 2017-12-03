#include "page_parser_helpers.h"

namespace CrawlerEngine
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
			WARNLOG << "Can't resolve absolute path: " << fixedPath;
			break;
		}
		
		fixedPath = fixedPath.left(prevSlashPos + 1) + fixedPath.right(fixedPath.size() - doublePointsPos - 4);
	}

	result.setPath(fixedPath);

	return result;
}

std::vector<QUrl> PageParserHelpers::resolveUrlList(const QUrl& baseUrl, 
	const std::vector<QUrl>& urlList) noexcept
{
	std::vector<QUrl> result;

	for (const QUrl& url : urlList)
	{
		result.push_back(resolveUrl(baseUrl, url));
	}

	return result;
}

std::vector<LinkInfo> PageParserHelpers::resolveUrlList(const QUrl& baseUrl,
	const std::vector<LinkInfo>& linkList) noexcept
{
	std::vector<LinkInfo> result = linkList;

	for (LinkInfo& link : result)
	{
		link.url = resolveUrl(baseUrl, link.url);
	}

	return result;
}

QUrl PageParserHelpers::resolveUrl(const QUrl& baseUrl, const QUrl& url) noexcept
{
	QUrl result = url;

	if (result.isRelative())
	{
		result = resolveRelativeUrl(result, baseUrl);
	}

	result.setFragment(QString());

	return result;
}

QUrl PageParserHelpers::removeUrlLastSlashIfExists(const QUrl& url)
{
	QString urlString = url.toDisplayString();

	if (urlString.endsWith("/"))
	{
		urlString = urlString.left(urlString.size() - 1);
	}

	return QUrl(urlString);
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

bool PageParserHelpers::isHtmlOrPlainContentType(const QString& contentType) noexcept
{
	return contentType.startsWith("text/html") ||
		contentType.startsWith("text/xhtml") ||
		contentType.startsWith("application/xhtml") ||
		contentType.startsWith("text/plain") ||
		contentType.isEmpty();
}

bool PageParserHelpers::isHttpOrHttpsScheme(const QUrl& url) noexcept
{
	return url.isRelative() || url.scheme() == QString("http") || url.scheme() == QString("https");
}


bool PageParserHelpers::isSubdomain(const QUrl& baseUrl, const QUrl& url)
{
	static const QRegularExpression s_regExp("(?:([a-z0-9\\.-]+)?\\.)?([a-z0-9]+\\.[a-z\\.]+){1,1}", 
		QRegularExpression::CaseInsensitiveOption);

	if (baseUrl.isRelative() || url.isRelative() || baseUrl == url)
	{
		return false;
	}

	QUrl baseUrlWithoutScheme = baseUrl;
	QUrl targetUrlWithoutScheme = url;

	baseUrlWithoutScheme.setScheme(QString::null);
	targetUrlWithoutScheme.setScheme(QString::null);

	const QRegularExpressionMatch baseUrlMatch = s_regExp.match(baseUrlWithoutScheme.host());
	const QRegularExpressionMatch targetUrlMatch = s_regExp.match(targetUrlWithoutScheme.host());

	const QStringList baseUrlCapturedTexts = baseUrlMatch.capturedTexts();
	const QStringList targetUrlCapturedTexts = targetUrlMatch.capturedTexts();

	if (targetUrlCapturedTexts.isEmpty() || baseUrlCapturedTexts.isEmpty())
	{
		WARNLOG << "Can't parse links";
		WARNLOG << "Base link:" << baseUrlWithoutScheme.host();
		WARNLOG << "Checking link:" << targetUrlWithoutScheme.host();

		return false;
	}

	QStringList baseUrlSubdomains = baseUrlCapturedTexts[1].split(".", QString::SkipEmptyParts);
	QStringList targetUrlSubdomains = targetUrlCapturedTexts[1].split(".", QString::SkipEmptyParts);

	baseUrlSubdomains.removeOne("www");
	targetUrlSubdomains.removeOne("www");

	if (baseUrlSubdomains.size() > targetUrlSubdomains.size() ||
		baseUrlSubdomains.isEmpty() && targetUrlSubdomains.isEmpty())
	{
		return false;
	}

	bool equal = false;
	
	for (int i = baseUrlSubdomains.size() - 1, j = targetUrlSubdomains.size() - 1; i >= 0 && j >= 0; --i, --j)
	{
		if (baseUrlSubdomains[i] == targetUrlSubdomains[j])
		{
			equal = true;
		}
		else
		{
			equal = false;
			break;
		}
	}

	return !equal;
}

}