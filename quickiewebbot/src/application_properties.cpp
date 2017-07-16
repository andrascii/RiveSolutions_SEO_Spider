#include "application_properties.h"

namespace QuickieWebBot
{

unsigned ApplicationProperties::threadCount() const
{
	return m_threadCount;
}

void ApplicationProperties::setThreadCount(unsigned value)
{
	m_threadCount = value;
	emit threadCountChanged();
}

bool ApplicationProperties::useProxy() const
{
	return m_useProxy;
}

void ApplicationProperties::setUseProxy(bool value)
{
	m_useProxy = value;
	emit useProxyChanged();
}

bool ApplicationProperties::proxyNeedAuthorization() const
{
	return m_proxyNeedAuthorization;
}

void ApplicationProperties::setProxyNeedAuthorization(bool value)
{
	m_proxyNeedAuthorization = value;
	emit proxyNeedAuthorizationChanged();
}

const QString& ApplicationProperties::proxyUsername() const
{
	return m_proxyUsername;
}

void ApplicationProperties::setProxyUsername(const QString& value)
{
	m_proxyUsername = value;
	emit proxyUsernameChanged();
}

const QString& ApplicationProperties::proxyPassword() const
{
	return m_proxyPassword;
}

void ApplicationProperties::setProxyPassword(const QString& value)
{
	m_proxyPassword = value;
	emit proxyPasswordChanged();
}

bool ApplicationProperties::checkImages() const
{
	return m_checkImages;
}

void ApplicationProperties::setCheckImages(bool value)
{
	m_checkImages = value;
	emit checkImagesChanged();
}

bool ApplicationProperties::checkJavaScript() const
{
	return m_checkJavaScript;
}

void ApplicationProperties::setCheckJavaScript(bool value)
{
	m_checkJavaScript = value;
	emit checkJavaScriptChanged();
}

bool ApplicationProperties::checkSWF() const
{
	return m_checkSWF;
}

void ApplicationProperties::setCheckSWF(bool value)
{
	m_checkSWF = value;
	emit checkSWFChanged();
}

bool ApplicationProperties::checkExternalUrls() const
{
	return m_checkExternalUrls;
}

void ApplicationProperties::setCheckExternalUrls(bool value)
{
	m_checkExternalUrls = value;
	emit checkExternalUrlsChanged();
}

bool ApplicationProperties::followInternalNoFollow() const
{
	return m_followInternalNoFollow;
}

void ApplicationProperties::setFollowInternalNoFollow(bool value)
{
	m_followInternalNoFollow = value;
	emit followInternalNoFollowChanged();
}

bool ApplicationProperties::followExternalNoFollow() const
{
	return m_followExternalNoFollow;
}

void ApplicationProperties::setFollowExternalNoFollow(bool value)
{
	m_followExternalNoFollow = value;
	emit followExternalNoFollowChanged();
}

bool ApplicationProperties::checkSubdomains() const
{
	return m_checkSubdomains;
}

void ApplicationProperties::setCheckSubdomains(bool value)
{
	m_checkSubdomains = value;
	emit checkSubdomainsChanged();
}

bool ApplicationProperties::checkCanonicals() const
{
	return m_checkCanonicals;
}

void ApplicationProperties::setCheckCanonicals(bool value)
{
	m_checkCanonicals = value;
	emit checkCanonicalsChanged();
}

bool ApplicationProperties::followRobotsTxt() const
{
	return m_followRobotsTxt;
}

void ApplicationProperties::setFollowRobotsTxt(bool value)
{
	m_followRobotsTxt = value;
	emit followRobotsTxtChanged();
}

const QString& ApplicationProperties::userAgent() const
{
	return m_userAgent;
}

void ApplicationProperties::setUserAgent(const QString& value)
{
	m_userAgent = value;
	emit userAgentChanged();
}

int ApplicationProperties::limitSearchTotal() const
{
	return m_limitSearchTotal;
}

void ApplicationProperties::setLimitSearchTotal(int value)
{
	m_limitSearchTotal = value;
	emit limitSearchTotalChanged();
}

int ApplicationProperties::limitMaxUrlLength() const
{
	return m_limitMaxUrlLength;
}

void ApplicationProperties::setLimitMaxUrlLength(int value)
{
	m_limitMaxUrlLength = value;
	emit limitMaxUrlLengthChanged();
}

int ApplicationProperties::limitTimeout() const
{
	return m_limitTimeout;
}

void ApplicationProperties::setLimitTimeout(int value)
{
	m_limitTimeout = value;
	emit limitTimeoutChanged();
}

int ApplicationProperties::maxRedirectCount() const
{
	return m_maxRedirectCount;
}

void ApplicationProperties::setMaxRedirectCount(int value)
{
	m_maxRedirectCount = value;
	emit maxRedirectCountChanged();
}

int ApplicationProperties::maxH1LengthChars() const
{
	return m_maxH1LengthChars;
}

void ApplicationProperties::setMaxH1LengthChars(int value)
{
	m_maxH1LengthChars = value;
	emit maxH1LengthCharsChanged();
}

int ApplicationProperties::maxH2LengthChars() const
{
	return m_maxH2LengthChars;
}

void ApplicationProperties::setMaxH2LengthChars(int value)
{
	m_maxH1LengthChars = value;
	emit maxH2LengthCharsChanged();
}

int ApplicationProperties::maxImageAltTextChars() const
{
	return m_maxImageAltTextChars;
}

void ApplicationProperties::setMaxImageAltTextChars(int value)
{
	m_maxImageAltTextChars = value;
	emit maxImageAltTextCharsChanged();
}

int ApplicationProperties::maxImageSize() const
{
	return m_maxImageSize;
}

void ApplicationProperties::setMaxImageSize(int value)
{
	m_maxImageSize = value;
	emit maxImageSizeChanged();
}

int ApplicationProperties::maxDescriptionWidth() const
{
	return m_maxDescriptionWidth;
}

void ApplicationProperties::setMaxDescriptionWidth(int value)
{
	m_maxDescriptionWidth = value;
	emit maxDescriptionWidthChanged();
}

int ApplicationProperties::maxTitleWidth() const
{
	return m_maxTitleWidth;
}

void ApplicationProperties::setMaxTitleWidth(int value)
{
	m_maxTitleWidth = value;
	emit maxTitleWidthChanged();
}

const QUrl& ApplicationProperties::host() const
{
	return m_host;
}

void ApplicationProperties::setHost(const QUrl& host)
{
	m_host = host;
	emit hostChanged();
}

ApplicationProperties::ApplicationProperties(QObject* parent)
	: QObject(parent)
{
}

}