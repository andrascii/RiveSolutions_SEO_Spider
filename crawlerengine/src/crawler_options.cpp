#include "crawler_options.h"

namespace CrawlerEngine
{

CrawlerOptions::CrawlerOptions(QObject* parent)
	: QObject(parent)
{
}

QObject* CrawlerOptions::qobject() const noexcept
{
	return const_cast<CrawlerOptions* const>(this);
}

Url CrawlerOptions::startCrawlingPage() const noexcept
{
	return m_data.startCrawlingPage;
}

void CrawlerOptions::setStartCrawlingPage(const Url& url)
{
	m_data.startCrawlingPage = url;

	emit startCrawlingPageChanged(m_data.startCrawlingPage);
}

int CrawlerOptions::limitMaxUrlLength() const noexcept
{
	return m_data.limitMaxUrlLength;
}

void CrawlerOptions::setLimitMaxUrlLength(int value) noexcept
{
	m_data.limitMaxUrlLength = value;

	emit limitMaxUrlLengthChanged(m_data.limitMaxUrlLength);
}

int CrawlerOptions::limitSearchTotal() const noexcept
{
	return m_data.limitSearchTotal;
}

void CrawlerOptions::setLimitSearchTotal(int value) noexcept
{
	m_data.limitSearchTotal = value;

	emit limitSearchTotalChanged(m_data.limitSearchTotal);
}

int CrawlerOptions::limitTimeout() const noexcept
{
	return m_data.limitTimeout;
}

void CrawlerOptions::setLimitTimeout(int value) noexcept
{
	m_data.limitTimeout = value;

	emit limitTimeoutChanged(m_data.limitTimeout);
}

int CrawlerOptions::maxRedirectsToFollow() const noexcept
{
	return m_data.maxRedirectsToFollow;
}

void CrawlerOptions::setMaxRedirectsToFollow(int value) noexcept
{
	m_data.maxRedirectsToFollow = value;

	emit maxRedirectsToFollowChanged(m_data.maxRedirectsToFollow);
}

int CrawlerOptions::maxLinksCountOnPage() const noexcept
{
	return m_data.maxLinksCountOnPage;
}

void CrawlerOptions::setMaxLinksCountOnPage(int value) noexcept
{
	m_data.maxLinksCountOnPage = value;

	emit maxLinksCountOnPageChanged(m_data.maxLinksCountOnPage);
}

int CrawlerOptions::maxTitleLength() const noexcept
{
	return m_data.maxTitleLength;
}

void CrawlerOptions::setMaxTitleLength(int value) noexcept
{
	m_data.maxLinksCountOnPage = value;

	emit maxLinksCountOnPageChanged(m_data.maxLinksCountOnPage);
}

int CrawlerOptions::minTitleLength() const noexcept
{
	return m_data.minTitleLength;
}

void CrawlerOptions::setMinTitleLength(int value) noexcept
{
	m_data.minTitleLength = value;

	emit minTitleLengthChanged(m_data.minTitleLength);
}

int CrawlerOptions::maxDescriptionLength() const noexcept
{
	return m_data.maxDescriptionLength;
}

void CrawlerOptions::setMaxDescriptionLength(int value) noexcept
{
	m_data.maxDescriptionLength = value;

	emit maxDescriptionLengthChanged(m_data.maxDescriptionLength);
}

int CrawlerOptions::minDescriptionLength() const noexcept
{
	return m_data.minDescriptionLength;
}

void CrawlerOptions::setMinDescriptionLength(int value) noexcept
{
	m_data.minDescriptionLength = value;

	emit minDescriptionLengthChanged(m_data.minDescriptionLength);
}

int CrawlerOptions::maxH1LengthChars() const noexcept
{
	return m_data.maxH1LengthChars;
}

void CrawlerOptions::setMaxH1LengthChars(int value) noexcept
{
	m_data.maxH1LengthChars = value;

	emit maxH1LengthCharsChanged(m_data.maxH1LengthChars);
}

int CrawlerOptions::maxH2LengthChars() const noexcept
{
	return m_data.maxH2LengthChars;
}

void CrawlerOptions::setMaxH2LengthChars(int value) noexcept
{
	m_data.maxH2LengthChars = value;

	emit maxH2LengthCharsChanged(m_data.maxH2LengthChars);
}

int CrawlerOptions::maxImageAltTextChars() const noexcept
{
	return m_data.maxImageAltTextChars;
}

void CrawlerOptions::setMaxImageAltTextChars(int value) noexcept
{
	m_data.maxImageAltTextChars = value;

	emit maxImageAltTextCharsChanged(m_data.maxImageAltTextChars);
}

int CrawlerOptions::maxImageSizeKb() const noexcept
{
	return m_data.maxImageSizeKb;
}

void CrawlerOptions::setMaxImageSizeKb(int value) noexcept
{
	m_data.maxImageSizeKb = value;
	
	emit maxImageSizeKbChanged(m_data.maxImageSizeKb);
}

int CrawlerOptions::maxPageSizeKb() const noexcept
{
	return m_data.maxPageSizeKb;
}

void CrawlerOptions::setMaxPageSizeKb(int value) noexcept
{
	m_data.maxPageSizeKb = value;

	emit maxPageSizeKbChanged(m_data.maxPageSizeKb);
}

bool CrawlerOptions::useProxy() const noexcept
{
	return m_data.useProxy;
}

void CrawlerOptions::setUseProxy(bool value) noexcept
{
	m_data.useProxy = value;

	emit useProxyChanged(m_data.useProxy);
}

QString CrawlerOptions::proxyHostName() const noexcept
{
	return m_data.proxyHostName;
}

void CrawlerOptions::setProxyHostName(const QString& value)
{
	m_data.proxyHostName = value;

	emit proxyHostNameChanged(m_data.proxyHostName);
}

int CrawlerOptions::proxyPort() const noexcept
{
	return m_data.proxyPort;
}

void CrawlerOptions::setProxyPort(int value) noexcept
{
	m_data.proxyPort = value;

	emit proxyPortChanged(m_data.proxyPort);
}

QString CrawlerOptions::proxyUser() const noexcept
{
	return m_data.proxyUser;
}

void CrawlerOptions::setProxyUser(const QString& value)
{
	m_data.proxyUser = value;

	emit proxyUserChanged(m_data.proxyUser);
}

QString CrawlerOptions::proxyPassword() const noexcept
{
	return m_data.proxyPassword;
}

void CrawlerOptions::setProxyPassword(const QString& value)
{
	m_data.proxyPassword = value;

	emit proxyPasswordChanged(m_data.proxyPassword);
}

bool CrawlerOptions::checkExternalLinks() const noexcept
{
	return m_data.checkExternalLinks;
}

void CrawlerOptions::setCheckExternalLinks(bool value) noexcept
{
	m_data.checkExternalLinks = value;

	emit checkExternalLinksChanged(m_data.checkExternalLinks);
}

bool CrawlerOptions::followInternalNofollow() const noexcept
{
	return m_data.followInternalNofollow;
}

void CrawlerOptions::setFollowInternalNofollow(bool value) noexcept
{
	m_data.followInternalNofollow = value;

	emit followInternalNofollowChanged(m_data.followInternalNofollow);
}

bool CrawlerOptions::followExternalNofollow() const noexcept
{
	return m_data.followExternalNofollow;
}

void CrawlerOptions::setFollowExternalNofollow(bool value) noexcept
{
	m_data.followExternalNofollow = value;

	emit followExternalNofollowChanged(m_data.followExternalNofollow);
}

bool CrawlerOptions::checkCanonicals() const noexcept
{
	return m_data.checkCanonicals;
}

void CrawlerOptions::setCheckCanonicals(bool value) noexcept
{
	m_data.checkCanonicals = value;

	emit checkCanonicalsChanged(m_data.checkCanonicals);
}

bool CrawlerOptions::checkSubdomains() const noexcept
{
	return m_data.checkSubdomains;
}

void CrawlerOptions::setCheckSubdomains(bool value) noexcept
{
	m_data.checkSubdomains = value;

	emit checkSubdomainsChanged(m_data.checkSubdomains);
}

bool CrawlerOptions::crawlOutsideOfStartFolder() const noexcept
{
	return m_data.crawlOutsideOfStartFolder;
}

void CrawlerOptions::setCrawlOutsideOfStartFolder(bool value) noexcept
{
	m_data.crawlOutsideOfStartFolder = value;

	emit crawlOutsideOfStartFolderChanged(m_data.crawlOutsideOfStartFolder);
}

bool CrawlerOptions::followRobotsTxtRules() const noexcept
{
	return m_data.followRobotsTxtRules;
}

void CrawlerOptions::setFollowRobotsTxtRules(bool value) noexcept
{
	m_data.followRobotsTxtRules = value;

	emit followRobotsTxtRulesChanged(m_data.followRobotsTxtRules);
}

UserAgentType CrawlerOptions::userAgentToFollow() const noexcept
{
	return m_data.userAgentToFollow;
}

void CrawlerOptions::setUserAgentToFollow(UserAgentType value) noexcept
{
	m_data.userAgentToFollow = value;

	emit userAgentToFollowChanged(m_data.userAgentToFollow);
}

ParserTypeFlags CrawlerOptions::parserTypeFlags() const noexcept
{
	return m_data.parserTypeFlags;
}

void CrawlerOptions::setParserTypeFlags(ParserTypeFlags value) noexcept
{
	m_data.parserTypeFlags = value;

	emit parserTypeFlagsChanged(m_data.parserTypeFlags);
}

int CrawlerOptions::pauseRangeFrom() const noexcept
{
	return m_data.pauseRangeFrom;
}

void CrawlerOptions::setPauseRangeFrom(int value) noexcept
{
	m_data.pauseRangeFrom = value;

	emit pauseRangeFromChanged(m_data.pauseRangeFrom);
}

int CrawlerOptions::pauseRangeTo() const noexcept
{
	return m_data.pauseRangeTo;
}

void CrawlerOptions::setPauseRangeTo(int value) noexcept
{
	m_data.pauseRangeTo = value;

	emit pauseRangeToChanged(m_data.pauseRangeTo);
}

QByteArray CrawlerOptions::userAgent() const noexcept
{
	return m_data.userAgent;
}

void CrawlerOptions::setUserAgent(const QByteArray& value)
{
	m_data.userAgent = value;

	emit userAgentChanged(m_data.userAgent);
}

const CrawlerOptionsData& CrawlerOptions::data() const noexcept
{
	return m_data;
}

void CrawlerOptions::setData(const CrawlerOptionsData& data) noexcept
{
	m_data = data;

	emit dataChanged(m_data);
}

void CrawlerOptions::setData(CrawlerOptionsData&& data) noexcept
{
	m_data = std::move(data);

	emit dataChanged(m_data);
}

}