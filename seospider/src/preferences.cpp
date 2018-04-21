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

		if (!property.isUser() && std::strcmp(property.name(), "objectName"))
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
	emit threadCountChanged(value);
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
	emit useProxyChanged(value);
}

bool Preferences::proxyNeedAuthorization() const
{
	return m_proxyNeedAuthorization;
}

void Preferences::setProxyNeedAuthorization(bool value)
{
	m_proxyNeedAuthorization = value;
	emit proxyNeedAuthorizationChanged(value);
}

const QString& Preferences::proxyUsername() const
{
	return m_proxyUsername;
}

void Preferences::setProxyUsername(const QString& value)
{
	m_proxyUsername = value;
	emit proxyUsernameChanged(value);
}

const QString& Preferences::proxyPassword() const
{
	return m_proxyPassword;
}

void Preferences::setProxyPassword(const QString& value)
{
	m_proxyPassword = value;
	emit proxyPasswordChanged(value);
}

const QString& Preferences::proxyAddress() const
{
	return m_proxyAddress;
}

void Preferences::setProxyAddress(const QString& value)
{
	m_proxyAddress = value;
	emit proxyAddressChanged(value);
}

int Preferences::proxyPort() const
{
	return m_proxyPort;
}

void Preferences::setProxyPort(int value)
{
	m_proxyPort = value;
	emit proxyPortChanged(value);
}

bool Preferences::checkImages() const
{
	return m_checkImages;
}

void Preferences::setCheckImages(bool value)
{
	m_checkImages = value;
	emit checkImagesChanged(value);
}

bool Preferences::checkCSS() const
{
	return m_checkCSS;
}

void Preferences::setCheckCSS(bool value)
{
	m_checkCSS = value;
	emit checkCSSChanged(value);
}

bool Preferences::checkJavaScript() const
{
	return m_checkJavaScript;
}

void Preferences::setCheckJavaScript(bool value)
{
	m_checkJavaScript = value;
	emit checkJavaScriptChanged(value);
}

bool Preferences::checkSWF() const
{
	return m_checkSWF;
}

void Preferences::setCheckSWF(bool value)
{
	m_checkSWF = value;
	emit checkSWFChanged(value);
}

bool Preferences::checkExternalUrls() const
{
	return m_checkExternalUrls;
}

void Preferences::setCheckExternalUrls(bool value)
{
	m_checkExternalUrls = value;
	emit checkExternalUrlsChanged(value);
}

bool Preferences::followInternalNoFollow() const
{
	return m_followInternalNoFollow;
}

void Preferences::setFollowInternalNoFollow(bool value)
{
	m_followInternalNoFollow = value;
	emit followInternalNoFollowChanged(value);
}

bool Preferences::followExternalNoFollow() const
{
	return m_followExternalNoFollow;
}

void Preferences::setFollowExternalNoFollow(bool value)
{
	m_followExternalNoFollow = value;
	emit followExternalNoFollowChanged(value);
}

bool Preferences::checkSubdomains() const
{
	return m_checkSubdomains;
}

void Preferences::setCheckSubdomains(bool value)
{
	m_checkSubdomains = value;
	emit checkSubdomainsChanged(value);
}

bool Preferences::checkCanonicals() const
{
	return m_checkCanonicals;
}

void Preferences::setCheckCanonicals(bool value)
{
	m_checkCanonicals = value;
	emit checkCanonicalsChanged(value);
}

bool Preferences::crawlOutsideOfStartFolder() const
{
	return m_crawlOutsideOfStartFolder;
}

void Preferences::setCrawlOutsideOfStartFolder(bool value)
{
	m_crawlOutsideOfStartFolder = value;
	emit crawlOutsideOfStartFolderChanged(value);
}

bool Preferences::usePauseTimer() const
{
	return m_usePauseTimer;
}

void Preferences::setUsePauseTimer(bool value)
{
	m_usePauseTimer = value;
	emit usePauseTimerChanged(value);
}

int Preferences::fromPauseTimer() const
{
	return m_fromPauseTimer;
}

void Preferences::setFromPauseTimer(int value)
{
	m_fromPauseTimer = value;
	emit fromPauseTimerChanged(value);
}

int Preferences::toPauseTimer() const
{
	return m_toPauseTimer;
}

void Preferences::setToPauseTimer(int value)
{
	m_toPauseTimer = value;
	emit toPauseTimerChanged(value);
}

bool Preferences::useCustomUserAgent() const
{
	return m_useCustomUserAgent;
}

bool Preferences::followRobotsTxt() const
{
	return m_followRobotsTxt;
}

void Preferences::setFollowRobotsTxt(bool value)
{
	m_followRobotsTxt = value;
	emit followRobotsTxtChanged(value);
}

int Preferences::robotSignature() const
{
	return m_robotSignature;
}

void Preferences::setRobotSignature(const QVariant& value)
{
	DEBUG_ASSERT(value.type() == QVariant::Int || value.type() == QVariant::String);

	m_robotSignature = value.toInt();
	emit robotSignatureChanged(value);
}

void Preferences::setUseCustomUserAgent(bool value)
{
	m_useCustomUserAgent = value;
	emit useCustomUserAgentChanged(value);
}

bool Preferences::useDesktopUserAgent() const
{
	return m_useDesktopUserAgent;
}

void Preferences::setUseDesktopUserAgent(bool value)
{
	m_useDesktopUserAgent = value;
	emit useDesktopUserAgentChanged(value);
}

bool Preferences::useMobileUserAgent() const
{
	return m_useMobileUserAgent;
}

void Preferences::setUseMobileUserAgent(bool value)
{
	m_useMobileUserAgent = value;
	emit useMobileUserAgentChanged(value);
}

const QString& Preferences::desktopUserAgent() const
{
	return m_desktopUserAgent;
}

void Preferences::setDesktopUserAgent(const QVariant& value)
{
	DEBUG_ASSERT(value.type() == QVariant::String); 

	m_desktopUserAgent = value.toString();
	emit desktopUserAgentChanged(value);
}

const QString& Preferences::mobileUserAgent() const
{
	return m_mobileUserAgent;
}

void Preferences::setMobileUserAgent(const QVariant& value)
{
	DEBUG_ASSERT(value.type() == QVariant::String);

	m_mobileUserAgent = value.toString();
	emit mobileUserAgentChanged(value);
}

int Preferences::limitSearchTotal() const
{
	return m_limitSearchTotal;
}

void Preferences::setLimitSearchTotal(int value)
{
	m_limitSearchTotal = value;
	emit limitSearchTotalChanged(value);
}

int Preferences::limitMaxUrlLength() const
{
	return m_limitMaxUrlLength;
}

void Preferences::setLimitMaxUrlLength(int value)
{
	m_limitMaxUrlLength = value;
	emit limitMaxUrlLengthChanged(value);
}

int Preferences::limitTimeout() const
{
	return m_limitTimeout;
}

void Preferences::setLimitTimeout(int value)
{
	m_limitTimeout = value;
	emit limitTimeoutChanged(value);
}

int Preferences::maxRedirectCount() const
{
	return m_maxRedirectCount;
}

void Preferences::setMaxRedirectCount(int value)
{
	m_maxRedirectCount = value;
	emit maxRedirectCountChanged(value);
}

int Preferences::maxH1LengthChars() const
{
	return m_maxH1LengthChars;
}

void Preferences::setMaxH1LengthChars(int value)
{
	m_maxH1LengthChars = value;
	emit maxH1LengthCharsChanged(value);
}

int Preferences::maxLinksCountOnPage() const
{
	return m_maxLinksCountOnPage;
}

void Preferences::setMaxLinksCountOnPage(int value)
{
	m_maxLinksCountOnPage = value;
}

int Preferences::maxH2LengthChars() const
{
	return m_maxH2LengthChars;
}

void Preferences::setMaxH2LengthChars(int value)
{
	m_maxH2LengthChars = value;
	emit maxH2LengthCharsChanged(value);
}

int Preferences::maxImageAltTextChars() const
{
	return m_maxImageAltTextChars;
}

void Preferences::setMaxImageAltTextChars(int value)
{
	m_maxImageAltTextChars = value;
	emit maxImageAltTextCharsChanged(value);
}

int Preferences::maxImageSize() const
{
	return m_maxImageSize;
}

void Preferences::setMaxImageSize(int value)
{
	m_maxImageSize = value;
	emit maxImageSizeChanged(value);
}

int Preferences::maxPageSize() const
{
	return m_maxPageSize;
}

void Preferences::setMaxPageSize(int value)
{
	m_maxPageSize = value;
	emit maxPageSizeChanged(value);
}

int Preferences::maxDescriptionLength() const
{
	return m_maxDescriptionLength;
}

void Preferences::setMaxDescriptionLength(int value)
{
	m_maxDescriptionLength = value;
	emit maxDescriptionLengthChanged(value);
}

int Preferences::minDescriptionLength() const
{
	return m_minDescriptionLength;
}

void Preferences::setMinDescriptionLength(int value)
{
	m_minDescriptionLength = value;
	emit minDescriptionLengthChanged(value);
}

int Preferences::maxTitleLength() const
{
	return m_maxTitleLength;
}

void Preferences::setMaxTitleLength(int value)
{
	m_maxTitleLength = value;
	emit maxTitleLengthChanged(value);
}

int Preferences::minTitleLength() const
{
	return m_minTitleLength;
}

void Preferences::setMinTitleLength(int value)
{
	m_minTitleLength = value;
	emit minTitleLengthChanged(value);
}

QString Preferences::companyName() const
{
	return m_companyName;
}

void Preferences::setCompanyName(QString value)
{
	m_companyName = value;
	emit companyNameChanged(value);
}

QString Preferences::companyEmail() const
{
	return m_companyEmail;
}

void Preferences::setCompanyEmail(QString value)
{
	m_companyEmail = value;
	emit companyEmailChanged(value);
}

QString Preferences::companyWebSite() const
{
	return m_companyWebSite;
}

void Preferences::setCompanyWebSite(QString value)
{
	m_companyWebSite = value;
	emit companyWebSiteChanged(value);
}

bool Preferences::useCustomColorForNotIndexedPages() const
{
	return m_useCustomColorForNotIndexedPages;
}

void Preferences::setUseCustomColorForNotIndexedPages(bool value)
{
	m_useCustomColorForNotIndexedPages = value;
	emit useCustomColorForNotIndexedPagesChanged(value);
}

QColor Preferences::notIndexedPagesColor() const
{
	return m_notIndexedPagesColor;
}

void Preferences::setNotIndexedPagesColor(QColor value)
{
	m_notIndexedPagesColor = value;
	emit notIndexedPagesColorChanged(value);
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