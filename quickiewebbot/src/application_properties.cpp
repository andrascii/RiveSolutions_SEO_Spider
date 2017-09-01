#include "application_properties.h"

namespace QuickieWebBot
{

ApplicationProperties::ApplicationProperties(QObject* parent)
	: QObject(parent)
	, m_minTitleLength(5)
	, m_maxTitleLength(50)
	, m_limitMaxUrlLength(50)
	, m_maxDescriptionLength(50)
	, m_minDescriptionLength(5)
	, m_maxH1LengthChars(50)
	, m_maxH2LengthChars(50)
	, m_maxImageAltTextChars(100)
	, m_maxImageSize(100)
	, m_proxyPort(80)
{
}

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

const QString& ApplicationProperties::proxyAddress() const
{
	return m_proxyAddress;
}

void ApplicationProperties::setProxyAddress(const QString& value)
{
	m_proxyAddress = value;
	emit proxyAddressChanged();
}

int ApplicationProperties::proxyPort() const
{
	return m_proxyPort;
}

void ApplicationProperties::setProxyPort(int value)
{
	m_proxyPort = value;
	emit proxyPortChanged();
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

bool ApplicationProperties::checkCSS() const
{
	return m_checkCSS;
}

void ApplicationProperties::setCheckCSS(bool value)
{
	m_checkCSS = value;
	emit checkCSSChanged();
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

int ApplicationProperties::maxDescriptionLength() const
{
	return m_maxDescriptionLength;
}

void ApplicationProperties::setMaxDescriptionLength(int value)
{
	m_maxDescriptionLength = value;
	emit maxDescriptionLengthChanged();
}

int ApplicationProperties::minDescriptionLength() const
{
	return m_minDescriptionLength;
}

void ApplicationProperties::setMinDescriptionLength(int value)
{
	m_minDescriptionLength = value;
	emit minDescriptionLengthChanged();
}

int ApplicationProperties::maxTitleLength() const
{
	return m_maxTitleLength;
}

void ApplicationProperties::setMaxTitleLength(int value)
{
	m_maxTitleLength = value;
	emit maxTitleLengthChanged();
}

int ApplicationProperties::minTitleLength() const
{
	return m_minTitleLength;
}

void ApplicationProperties::setMinTitleLength(int value)
{
	m_minTitleLength = value;
	emit minTitleLengthChanged();
}

const QUrl& ApplicationProperties::url() const
{
	return m_url;
}

void ApplicationProperties::setUrl(const QUrl& url)
{
	m_url = url;

	if (m_url.scheme().isEmpty())
	{
		DEBUGLOG << "Corrected scheme of passed URL";

		m_url.setUrl("http://" + url.toString());
	}

	emit urlChanged();
}

}