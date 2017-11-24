#include "preferences.h"
#include "isettings_accessor.h"

namespace SeoSpider
{

Preferences::Preferences(ISettingsAccessor* settingsAccessor, QObject* parent)
	: QObject(parent)
	, m_settingsAccessor(settingsAccessor)
{
}

Preferences::~Preferences()
{
	for(int i = 0; i < metaObject()->propertyCount(); ++i)
	{
		const QMetaProperty property = metaObject()->property(i);

		if (!property.isUser() && property.name() != "objectName")
		{
			m_settingsAccessor->saveToSettings(property.name(), property.read(this));
		}
	}
}

unsigned Preferences::threadCount() const
{
	return m_threadCount;
}

void Preferences::setThreadCount(unsigned value)
{
	m_threadCount = value;
	emit threadCountChanged();
}

QString Preferences::applicationLanguage() const
{
	return m_applicationLanguage;
}

void Preferences::setApplicationLanguage(const QString& value)
{
	m_applicationLanguage = value;
	emit applicationLanguageChanged(value);
}

bool Preferences::useProxy() const
{
	return m_useProxy;
}

void Preferences::setUseProxy(bool value)
{
	m_useProxy = value;
	emit useProxyChanged();
}

bool Preferences::proxyNeedAuthorization() const
{
	return m_proxyNeedAuthorization;
}

void Preferences::setProxyNeedAuthorization(bool value)
{
	m_proxyNeedAuthorization = value;
	emit proxyNeedAuthorizationChanged();
}

const QString& Preferences::proxyUsername() const
{
	return m_proxyUsername;
}

void Preferences::setProxyUsername(const QString& value)
{
	m_proxyUsername = value;
	emit proxyUsernameChanged();
}

const QString& Preferences::proxyPassword() const
{
	return m_proxyPassword;
}

void Preferences::setProxyPassword(const QString& value)
{
	m_proxyPassword = value;
	emit proxyPasswordChanged();
}

const QString& Preferences::proxyAddress() const
{
	return m_proxyAddress;
}

void Preferences::setProxyAddress(const QString& value)
{
	m_proxyAddress = value;
	emit proxyAddressChanged();
}

int Preferences::proxyPort() const
{
	return m_proxyPort;
}

void Preferences::setProxyPort(int value)
{
	m_proxyPort = value;
	emit proxyPortChanged();
}

bool Preferences::checkImages() const
{
	return m_checkImages;
}

void Preferences::setCheckImages(bool value)
{
	m_checkImages = value;
	emit checkImagesChanged();
}

bool Preferences::checkCSS() const
{
	return m_checkCSS;
}

void Preferences::setCheckCSS(bool value)
{
	m_checkCSS = value;
	emit checkCSSChanged();
}

bool Preferences::checkJavaScript() const
{
	return m_checkJavaScript;
}

void Preferences::setCheckJavaScript(bool value)
{
	m_checkJavaScript = value;
	emit checkJavaScriptChanged();
}

bool Preferences::checkSWF() const
{
	return m_checkSWF;
}

void Preferences::setCheckSWF(bool value)
{
	m_checkSWF = value;
	emit checkSWFChanged();
}

bool Preferences::checkExternalUrls() const
{
	return m_checkExternalUrls;
}

void Preferences::setCheckExternalUrls(bool value)
{
	m_checkExternalUrls = value;
	emit checkExternalUrlsChanged();
}

bool Preferences::followInternalNoFollow() const
{
	return m_followInternalNoFollow;
}

void Preferences::setFollowInternalNoFollow(bool value)
{
	m_followInternalNoFollow = value;
	emit followInternalNoFollowChanged();
}

bool Preferences::followExternalNoFollow() const
{
	return m_followExternalNoFollow;
}

void Preferences::setFollowExternalNoFollow(bool value)
{
	m_followExternalNoFollow = value;
	emit followExternalNoFollowChanged();
}

bool Preferences::checkSubdomains() const
{
	return m_checkSubdomains;
}

void Preferences::setCheckSubdomains(bool value)
{
	m_checkSubdomains = value;
	emit checkSubdomainsChanged();
}

bool Preferences::checkCanonicals() const
{
	return m_checkCanonicals;
}

void Preferences::setCheckCanonicals(bool value)
{
	m_checkCanonicals = value;
	emit checkCanonicalsChanged();
}

bool Preferences::followRobotsTxt() const
{
	return m_followRobotsTxt;
}

void Preferences::setFollowRobotsTxt(bool value)
{
	m_followRobotsTxt = value;
	emit followRobotsTxtChanged();
}

const QString& Preferences::userAgent() const
{
	return m_userAgent;
}

void Preferences::setUserAgent(const QString& value)
{
	m_userAgent = value;
	emit userAgentChanged();
}

int Preferences::limitSearchTotal() const
{
	return m_limitSearchTotal;
}

void Preferences::setLimitSearchTotal(int value)
{
	m_limitSearchTotal = value;
	emit limitSearchTotalChanged();
}

int Preferences::limitMaxUrlLength() const
{
	return m_limitMaxUrlLength;
}

void Preferences::setLimitMaxUrlLength(int value)
{
	m_limitMaxUrlLength = value;
	emit limitMaxUrlLengthChanged();
}

int Preferences::limitTimeout() const
{
	return m_limitTimeout;
}

void Preferences::setLimitTimeout(int value)
{
	m_limitTimeout = value;
	emit limitTimeoutChanged();
}

int Preferences::maxRedirectCount() const
{
	return m_maxRedirectCount;
}

void Preferences::setMaxRedirectCount(int value)
{
	m_maxRedirectCount = value;
	emit maxRedirectCountChanged();
}

int Preferences::maxH1LengthChars() const
{
	return m_maxH1LengthChars;
}

void Preferences::setMaxH1LengthChars(int value)
{
	m_maxH1LengthChars = value;
	emit maxH1LengthCharsChanged();
}

int Preferences::maxH2LengthChars() const
{
	return m_maxH2LengthChars;
}

void Preferences::setMaxH2LengthChars(int value)
{
	m_maxH2LengthChars = value;
	emit maxH2LengthCharsChanged();
}

int Preferences::maxImageAltTextChars() const
{
	return m_maxImageAltTextChars;
}

void Preferences::setMaxImageAltTextChars(int value)
{
	m_maxImageAltTextChars = value;
	emit maxImageAltTextCharsChanged();
}

int Preferences::maxImageSize() const
{
	return m_maxImageSize;
}

void Preferences::setMaxImageSize(int value)
{
	m_maxImageSize = value;
	emit maxImageSizeChanged();
}

int Preferences::maxDescriptionLength() const
{
	return m_maxDescriptionLength;
}

void Preferences::setMaxDescriptionLength(int value)
{
	m_maxDescriptionLength = value;
	emit maxDescriptionLengthChanged();
}

int Preferences::minDescriptionLength() const
{
	return m_minDescriptionLength;
}

void Preferences::setMinDescriptionLength(int value)
{
	m_minDescriptionLength = value;
	emit minDescriptionLengthChanged();
}

int Preferences::maxTitleLength() const
{
	return m_maxTitleLength;
}

void Preferences::setMaxTitleLength(int value)
{
	m_maxTitleLength = value;
	emit maxTitleLengthChanged();
}

int Preferences::minTitleLength() const
{
	return m_minTitleLength;
}

void Preferences::setMinTitleLength(int value)
{
	m_minTitleLength = value;
	emit minTitleLengthChanged();
}

const QUrl& Preferences::url() const
{
	return m_url;
}

void Preferences::setUrl(const QUrl& url)
{
	m_url = url;

	if (m_url.scheme().isEmpty())
	{
		DEBUGLOG << "Corrected scheme of passed URL";

		m_url.setUrl("http://" + url.toString());
	}

	emit urlChanged();
}

void Preferences::addDefaultProperty(const QByteArray& key, const QVariant& defaultValue) noexcept
{
	const QVariant valueFromSettings = m_settingsAccessor->loadFromSettings(key, defaultValue);
	m_settingsAccessor->saveToSettings(key.constData(), valueFromSettings);

	setProperty(key, valueFromSettings);
}

void Preferences::load()
{
	QFile config(":/config/defaults.cfg");
	config.open(QIODevice::ReadOnly);

	if (!config.isOpen())
	{
		ERRLOG << "Can't read defaults!";
	}
	else
	{
		readDefaults(config.readAll());
	}

	for(const std::pair<const QString, QVariant>& pair : m_defaults)
	{
		addDefaultProperty(pair.first.toLatin1(), pair.second);
	}
}

void Preferences::readDefaults(const QString& str)
{
	QRegExp rxRecord("^\\s*(((\\w+)/)?(\\w+))\\s*:\\s*([^\\s].{0,})\\b\\s*$");

	QStringList keyValues = str.split(QRegExp(";\\W*"), QString::SkipEmptyParts);

	foreach(const QString& keyValue, keyValues)
	{
		if (rxRecord.indexIn(keyValue) == -1)
		{
			continue;
		}

		const QString group = rxRecord.cap(3);
		const QString key = rxRecord.cap(4);
		const QString value = rxRecord.cap(5);
			
		if (!key.isEmpty())
		{
			m_defaults[rxRecord.cap(1)] = value;
		}
	}
}

}