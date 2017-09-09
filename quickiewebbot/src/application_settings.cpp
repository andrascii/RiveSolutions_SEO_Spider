#include "application_settings.h"

namespace QuickieWebBot
{

ApplicationSettings::ApplicationSettings(QObject* parent)
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

unsigned ApplicationSettings::threadCount() const
{
	return m_threadCount;
}

void ApplicationSettings::setThreadCount(unsigned value)
{
	m_threadCount = value;
	emit threadCountChanged();
}

bool ApplicationSettings::useProxy() const
{
	return m_useProxy;
}

void ApplicationSettings::setUseProxy(bool value)
{
	m_useProxy = value;
	emit useProxyChanged();
}

bool ApplicationSettings::proxyNeedAuthorization() const
{
	return m_proxyNeedAuthorization;
}

void ApplicationSettings::setProxyNeedAuthorization(bool value)
{
	m_proxyNeedAuthorization = value;
	emit proxyNeedAuthorizationChanged();
}

const QString& ApplicationSettings::proxyUsername() const
{
	return m_proxyUsername;
}

void ApplicationSettings::setProxyUsername(const QString& value)
{
	m_proxyUsername = value;
	emit proxyUsernameChanged();
}

const QString& ApplicationSettings::proxyPassword() const
{
	return m_proxyPassword;
}

void ApplicationSettings::setProxyPassword(const QString& value)
{
	m_proxyPassword = value;
	emit proxyPasswordChanged();
}

const QString& ApplicationSettings::proxyAddress() const
{
	return m_proxyAddress;
}

void ApplicationSettings::setProxyAddress(const QString& value)
{
	m_proxyAddress = value;
	emit proxyAddressChanged();
}

int ApplicationSettings::proxyPort() const
{
	return m_proxyPort;
}

void ApplicationSettings::setProxyPort(int value)
{
	m_proxyPort = value;
	emit proxyPortChanged();
}

bool ApplicationSettings::checkImages() const
{
	return m_checkImages;
}

void ApplicationSettings::setCheckImages(bool value)
{
	m_checkImages = value;
	emit checkImagesChanged();
}

bool ApplicationSettings::checkCSS() const
{
	return m_checkCSS;
}

void ApplicationSettings::setCheckCSS(bool value)
{
	m_checkCSS = value;
	emit checkCSSChanged();
}

bool ApplicationSettings::checkJavaScript() const
{
	return m_checkJavaScript;
}

void ApplicationSettings::setCheckJavaScript(bool value)
{
	m_checkJavaScript = value;
	emit checkJavaScriptChanged();
}

bool ApplicationSettings::checkSWF() const
{
	return m_checkSWF;
}

void ApplicationSettings::setCheckSWF(bool value)
{
	m_checkSWF = value;
	emit checkSWFChanged();
}

bool ApplicationSettings::checkExternalUrls() const
{
	return m_checkExternalUrls;
}

void ApplicationSettings::setCheckExternalUrls(bool value)
{
	m_checkExternalUrls = value;
	emit checkExternalUrlsChanged();
}

bool ApplicationSettings::followInternalNoFollow() const
{
	return m_followInternalNoFollow;
}

void ApplicationSettings::setFollowInternalNoFollow(bool value)
{
	m_followInternalNoFollow = value;
	emit followInternalNoFollowChanged();
}

bool ApplicationSettings::followExternalNoFollow() const
{
	return m_followExternalNoFollow;
}

void ApplicationSettings::setFollowExternalNoFollow(bool value)
{
	m_followExternalNoFollow = value;
	emit followExternalNoFollowChanged();
}

bool ApplicationSettings::checkSubdomains() const
{
	return m_checkSubdomains;
}

void ApplicationSettings::setCheckSubdomains(bool value)
{
	m_checkSubdomains = value;
	emit checkSubdomainsChanged();
}

bool ApplicationSettings::checkCanonicals() const
{
	return m_checkCanonicals;
}

void ApplicationSettings::setCheckCanonicals(bool value)
{
	m_checkCanonicals = value;
	emit checkCanonicalsChanged();
}

bool ApplicationSettings::followRobotsTxt() const
{
	return m_followRobotsTxt;
}

void ApplicationSettings::setFollowRobotsTxt(bool value)
{
	m_followRobotsTxt = value;
	emit followRobotsTxtChanged();
}

const QString& ApplicationSettings::userAgent() const
{
	return m_userAgent;
}

void ApplicationSettings::setUserAgent(const QString& value)
{
	m_userAgent = value;
	emit userAgentChanged();
}

int ApplicationSettings::limitSearchTotal() const
{
	return m_limitSearchTotal;
}

void ApplicationSettings::setLimitSearchTotal(int value)
{
	m_limitSearchTotal = value;
	emit limitSearchTotalChanged();
}

int ApplicationSettings::limitMaxUrlLength() const
{
	return m_limitMaxUrlLength;
}

void ApplicationSettings::setLimitMaxUrlLength(int value)
{
	m_limitMaxUrlLength = value;
	emit limitMaxUrlLengthChanged();
}

int ApplicationSettings::limitTimeout() const
{
	return m_limitTimeout;
}

void ApplicationSettings::setLimitTimeout(int value)
{
	m_limitTimeout = value;
	emit limitTimeoutChanged();
}

int ApplicationSettings::maxRedirectCount() const
{
	return m_maxRedirectCount;
}

void ApplicationSettings::setMaxRedirectCount(int value)
{
	m_maxRedirectCount = value;
	emit maxRedirectCountChanged();
}

int ApplicationSettings::maxH1LengthChars() const
{
	return m_maxH1LengthChars;
}

void ApplicationSettings::setMaxH1LengthChars(int value)
{
	m_maxH1LengthChars = value;
	emit maxH1LengthCharsChanged();
}

int ApplicationSettings::maxH2LengthChars() const
{
	return m_maxH2LengthChars;
}

void ApplicationSettings::setMaxH2LengthChars(int value)
{
	m_maxH1LengthChars = value;
	emit maxH2LengthCharsChanged();
}

int ApplicationSettings::maxImageAltTextChars() const
{
	return m_maxImageAltTextChars;
}

void ApplicationSettings::setMaxImageAltTextChars(int value)
{
	m_maxImageAltTextChars = value;
	emit maxImageAltTextCharsChanged();
}

int ApplicationSettings::maxImageSize() const
{
	return m_maxImageSize;
}

void ApplicationSettings::setMaxImageSize(int value)
{
	m_maxImageSize = value;
	emit maxImageSizeChanged();
}

int ApplicationSettings::maxDescriptionLength() const
{
	return m_maxDescriptionLength;
}

void ApplicationSettings::setMaxDescriptionLength(int value)
{
	m_maxDescriptionLength = value;
	emit maxDescriptionLengthChanged();
}

int ApplicationSettings::minDescriptionLength() const
{
	return m_minDescriptionLength;
}

void ApplicationSettings::setMinDescriptionLength(int value)
{
	m_minDescriptionLength = value;
	emit minDescriptionLengthChanged();
}

int ApplicationSettings::maxTitleLength() const
{
	return m_maxTitleLength;
}

void ApplicationSettings::setMaxTitleLength(int value)
{
	m_maxTitleLength = value;
	emit maxTitleLengthChanged();
}

int ApplicationSettings::minTitleLength() const
{
	return m_minTitleLength;
}

void ApplicationSettings::setMinTitleLength(int value)
{
	m_minTitleLength = value;
	emit minTitleLengthChanged();
}

const QUrl& ApplicationSettings::url() const
{
	return m_url;
}

void ApplicationSettings::setUrl(const QUrl& url)
{
	m_url = url;

	if (m_url.scheme().isEmpty())
	{
		DEBUGLOG << "Corrected scheme of passed URL";

		m_url.setUrl("http://" + url.toString());
	}

	emit urlChanged();
}

QVariant ApplicationSettings::get(QString key, SettingsGroup group )
{
	// m_applicationSettings[group].value(key, 0);
	return 0;
}

void ApplicationSettings::set(QString key, QVariant value, SettingsGroup group)
{
	//m_applicationSettings[group].setValue(key, value);
}

void ApplicationSettings::setDefaults(const QString &str)
{

}

}