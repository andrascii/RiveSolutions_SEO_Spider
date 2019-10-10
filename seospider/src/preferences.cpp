#include "preferences.h"
#include "isettings_accessor.h"

namespace
{

using namespace SeoSpider;

const QMap<QString, QString> s_languageLocales
{
	{ "English", "en_EN" },
	{ "Russian", "ru_RU" },
};

}

namespace SeoSpider
{

Preferences::Preferences(ISettingsAccessor* settingsAccessor, QObject* parent)
	: QObject(parent)
	, m_settingsAccessor(settingsAccessor)
{
    std::fill(m_extractorRuleTypes.begin(), m_extractorRuleTypes.end(), -1);
    std::fill(m_extractionTypes.begin(), m_extractionTypes.end(), 0);
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

QString Preferences::localeFromString(const QString& language)
{
	ASSERT(s_languageLocales.contains(language));

	return s_languageLocales[language];
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

bool Preferences::checkVideo() const
{
	return m_checkVideo;
}

void Preferences::setCheckVideo(bool value)
{
	m_checkVideo = value;
	emit checkVideoChanged(value);
}

bool Preferences::checkOther() const
{
	return m_checkOther;
}

void Preferences::setCheckOther(bool value)
{
	m_checkOther = value;
	emit checkOtherChanged(value);
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

bool Preferences::crawlMetaHrefLangLinks() const
{
	return m_crawlMetaHrefLangLinks;
}

void Preferences::setCrawlMetaHrefLangLinks(bool value)
{
	m_crawlMetaHrefLangLinks = value;
	emit crawlMetaHrefLangLinksChanged(value);
}

QString Preferences::excludeUrlRegExps() const
{
	return m_excludedUrlRegExps;
}

void Preferences::setExcludeUrlRegExps(const QString& value)
{
	m_excludedUrlRegExps = value;
	emit excludeUrlRegExpsChanged(value);
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

int Preferences::maxParallelConnections() const
{
	return m_maxParallelConnections;
}

void Preferences::setMaxParallelConnections(int value)
{
	m_maxParallelConnections = value;
	emit maxParallelConnectionsChanged(value);
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
	emit maxLinksCountOnPageChanged(m_maxLinksCountOnPage);
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

bool Preferences::searchYandexMetricaCounters() const
{
	return m_searchYandexMetricaCounters;
}

void Preferences::setSearchYandexMetricaCounters(bool value)
{
	m_searchYandexMetricaCounters = value;
	emit searchYandexMetricaCountersChanged(m_searchYandexMetricaCounters);
}

bool Preferences::searchYandexMetricaCounter1() const
{
	return m_searchYandexMetricaCounter1;
}

void Preferences::setSearchYandexMetricaCounter1(bool value)
{
	m_searchYandexMetricaCounter1 = value;
	emit searchYandexMetricaCounter1Changed(m_searchYandexMetricaCounter1);
}

int Preferences::yandexMetricaCounter1Id() const
{
	return m_yandexMetricaCounter1Id;
}

void Preferences::setYandexMetricaCounter1Id(int value)
{
	m_yandexMetricaCounter1Id = value;
	emit yandexMetricaCounter1IdChanged(m_yandexMetricaCounter1Id);
}

bool Preferences::searchYandexMetricaCounter2() const
{
	return m_searchYandexMetricaCounter2;
}

void Preferences::setSearchYandexMetricaCounter2(bool value)
{
	m_searchYandexMetricaCounter2 = value;
	emit searchYandexMetricaCounter2Changed(m_searchYandexMetricaCounter2);
}

int Preferences::yandexMetricaCounter2Id() const
{
	return m_yandexMetricaCounter2Id;
}

void Preferences::setYandexMetricaCounter2Id(int value)
{
	m_yandexMetricaCounter2Id = value;
	emit yandexMetricaCounter2IdChanged(m_yandexMetricaCounter2Id);
}

bool Preferences::searchYandexMetricaCounter3() const
{
	return m_searchYandexMetricaCounter3;
}

void Preferences::setSearchYandexMetricaCounter3(bool value)
{
	m_searchYandexMetricaCounter3 = value;
	emit searchYandexMetricaCounter3Changed(m_searchYandexMetricaCounter3);
}

int Preferences::yandexMetricaCounter3Id() const
{
	return m_yandexMetricaCounter3Id;
}

void Preferences::setYandexMetricaCounter3Id(int value)
{
	m_yandexMetricaCounter3Id = value;
	emit yandexMetricaCounter3IdChanged(m_yandexMetricaCounter3Id);
}

bool Preferences::searchYandexMetricaCounter4() const
{
	return m_searchYandexMetricaCounter4;
}

void Preferences::setSearchYandexMetricaCounter4(bool value)
{
	m_searchYandexMetricaCounter4 = value;
	emit searchYandexMetricaCounter4Changed(m_searchYandexMetricaCounter4);
}

int Preferences::yandexMetricaCounter4Id() const
{
	return m_yandexMetricaCounter4Id;
}

void Preferences::setYandexMetricaCounter4Id(int value)
{
	m_yandexMetricaCounter4Id = value;
	emit yandexMetricaCounter4IdChanged(m_yandexMetricaCounter4Id);
}

bool Preferences::searchYandexMetricaCounter5() const
{
	return m_searchYandexMetricaCounter5;
}

void Preferences::setSearchYandexMetricaCounter5(bool value)
{
	m_searchYandexMetricaCounter5 = value;
	emit searchYandexMetricaCounter5Changed(m_searchYandexMetricaCounter5);
}

int Preferences::yandexMetricaCounter5Id() const
{
	return m_yandexMetricaCounter5Id;
}

void Preferences::setYandexMetricaCounter5Id(int value)
{
	m_yandexMetricaCounter5Id = value;
	emit yandexMetricaCounter5IdChanged(m_yandexMetricaCounter5Id);
}

bool Preferences::searchGoogleAnalyticsCounters() const
{
	return m_searchGoogleAnalyticsCounters;
}

void Preferences::setSearchGoogleAnalyticsCounters(bool value)
{
	m_searchGoogleAnalyticsCounters = value;
	emit searchGoogleAnalyticsCountersChanged(m_searchGoogleAnalyticsCounters);
}

bool Preferences::searchGoogleAnalyticsCounter1() const
{
	return m_searchGoogleAnalyticsCounter1;
}

void Preferences::setSearchGoogleAnalyticsCounter1(bool value)
{
	m_searchGoogleAnalyticsCounter1 = value;
	emit searchGoogleAnalyticsCounter1Changed(m_searchGoogleAnalyticsCounter1);
}

const QString& Preferences::googleAnalyticsCounter1Id() const
{
	return m_googleAnalyticsCounter1Id;
}

void Preferences::setGoogleAnalyticsCounter1Id(const QString& value)
{
	m_googleAnalyticsCounter1Id = value;
	emit googleAnalyticsCounter1IdChanged(m_googleAnalyticsCounter1Id);
}

bool Preferences::searchGoogleAnalyticsCounter2() const
{
	return m_searchGoogleAnalyticsCounter2;
}

void Preferences::setSearchGoogleAnalyticsCounter2(bool value)
{
	m_searchGoogleAnalyticsCounter2 = value;
	emit searchGoogleAnalyticsCounter2Changed(m_searchGoogleAnalyticsCounter2);
}

const QString& Preferences::googleAnalyticsCounter2Id() const
{
	return m_googleAnalyticsCounter2Id;
}

void Preferences::setGoogleAnalyticsCounter2Id(const QString& value)
{
	m_googleAnalyticsCounter2Id = value;
	emit googleAnalyticsCounter2IdChanged(m_googleAnalyticsCounter2Id);
}

bool Preferences::searchGoogleAnalyticsCounter3() const
{
	return m_searchGoogleAnalyticsCounter3;
}

void Preferences::setSearchGoogleAnalyticsCounter3(bool value)
{
	m_searchGoogleAnalyticsCounter3 = value;
	emit searchGoogleAnalyticsCounter3Changed(m_searchGoogleAnalyticsCounter3);
}

const QString& Preferences::googleAnalyticsCounter3Id() const
{
	return m_googleAnalyticsCounter3Id;
}

void Preferences::setGoogleAnalyticsCounter3Id(const QString& value)
{
	m_googleAnalyticsCounter3Id = value;
	emit googleAnalyticsCounter3IdChanged(m_googleAnalyticsCounter3Id);
}

bool Preferences::searchGoogleAnalyticsCounter4() const
{
	return m_searchGoogleAnalyticsCounter4;
}

void Preferences::setSearchGoogleAnalyticsCounter4(bool value)
{
	m_searchGoogleAnalyticsCounter4 = value;
	emit searchGoogleAnalyticsCounter4Changed(m_searchGoogleAnalyticsCounter4);
}

const QString& Preferences::googleAnalyticsCounter4Id() const
{
	return m_googleAnalyticsCounter4Id;
}

void Preferences::setGoogleAnalyticsCounter4Id(const QString& value)
{
	m_googleAnalyticsCounter4Id = value;
	emit googleAnalyticsCounter4IdChanged(m_googleAnalyticsCounter4Id);
}

bool Preferences::searchGoogleAnalyticsCounter5() const
{
	return m_searchGoogleAnalyticsCounter5;
}

void Preferences::setSearchGoogleAnalyticsCounter5(bool value)
{
	m_searchGoogleAnalyticsCounter5 = value;
	emit searchGoogleAnalyticsCounter5Changed(m_searchGoogleAnalyticsCounter5);
}

const QString& Preferences::googleAnalyticsCounter5Id() const
{
	return m_googleAnalyticsCounter5Id;
}

void Preferences::setGoogleAnalyticsCounter5Id(const QString& value)
{
	m_googleAnalyticsCounter5Id = value;
	emit googleAnalyticsCounter5IdChanged(m_googleAnalyticsCounter5Id);
}

const QString& Preferences::extractorName1() const
{
    return m_extractorNames[0];
}

const QString& Preferences::extractorName2() const
{
    return m_extractorNames[1];
}

const QString& Preferences::extractorName3() const
{
    return m_extractorNames[2];
}

const QString& Preferences::extractorName4() const
{
    return m_extractorNames[3];
}

const QString& Preferences::extractorName5() const
{
    return m_extractorNames[4];
}

const QString& Preferences::extractorName6() const
{
    return m_extractorNames[5];
}

const QString& Preferences::extractorName7() const
{
    return m_extractorNames[6];
}

const QString& Preferences::extractorName8() const
{
    return m_extractorNames[7];
}

const QString& Preferences::extractorName9() const
{
    return m_extractorNames[8];
}

const QString& Preferences::extractorName10() const
{
    return m_extractorNames[9];
}

void Preferences::setExtractorName1(const QString& value)
{
    m_extractorNames[0] = value;
    emit extractorName1Changed(value);
}

void Preferences::setExtractorName2(const QString& value)
{
    m_extractorNames[1] = value;
    emit extractorName2Changed(value);
}

void Preferences::setExtractorName3(const QString& value)
{
    m_extractorNames[2] = value;
    emit extractorName3Changed(value);
}

void Preferences::setExtractorName4(const QString& value)
{
    m_extractorNames[3] = value;
    emit extractorName4Changed(value);
}

void Preferences::setExtractorName5(const QString& value)
{
    m_extractorNames[4] = value;
    emit extractorName5Changed(value);
}

void Preferences::setExtractorName6(const QString& value)
{
    m_extractorNames[5] = value;
    emit extractorName6Changed(value);
}

void Preferences::setExtractorName7(const QString& value)
{
    m_extractorNames[6] = value;
    emit extractorName7Changed(value);
}

void Preferences::setExtractorName8(const QString& value)
{
    m_extractorNames[7] = value;
    emit extractorName8Changed(value);
}

void Preferences::setExtractorName9(const QString& value)
{
    m_extractorNames[8] = value;
    emit extractorName9Changed(value);
}

void Preferences::setExtractorName10(const QString& value)
{
    m_extractorNames[9] = value;
    emit extractorName10Changed(value);
}

int Preferences::extractorRuleType1() const
{
    return m_extractorRuleTypes[0];
}

int Preferences::extractorRuleType2() const
{
    return m_extractorRuleTypes[1];
}

int Preferences::extractorRuleType3() const
{
    return m_extractorRuleTypes[2];
}

int Preferences::extractorRuleType4() const
{
    return m_extractorRuleTypes[3];
}

int Preferences::extractorRuleType5() const
{
    return m_extractorRuleTypes[4];
}

int Preferences::extractorRuleType6() const
{
    return m_extractorRuleTypes[5];
}

int Preferences::extractorRuleType7() const
{
    return m_extractorRuleTypes[6];
}

int Preferences::extractorRuleType8() const
{
    return m_extractorRuleTypes[7];
}

int Preferences::extractorRuleType9() const
{
    return m_extractorRuleTypes[8];
}

int Preferences::extractorRuleType10() const
{
    return m_extractorRuleTypes[9];
}

void Preferences::setExtractorRuleType1(int value)
{
    m_extractorRuleTypes[0] = value;
    emit extractorRuleType1Changed(value);
}

void Preferences::setExtractorRuleType2(int value)
{
    m_extractorRuleTypes[1] = value;
    emit extractorRuleType2Changed(value);
}

void Preferences::setExtractorRuleType3(int value)
{
    m_extractorRuleTypes[2] = value;
    emit extractorRuleType3Changed(value);
}

void Preferences::setExtractorRuleType4(int value)
{
    m_extractorRuleTypes[3] = value;
    emit extractorRuleType4Changed(value);
}

void Preferences::setExtractorRuleType5(int value)
{
    m_extractorRuleTypes[4] = value;
    emit extractorRuleType5Changed(value);
}

void Preferences::setExtractorRuleType6(int value)
{
    m_extractorRuleTypes[5] = value;
    emit extractorRuleType6Changed(value);
}

void Preferences::setExtractorRuleType7(int value)
{
    m_extractorRuleTypes[6] = value;
    emit extractorRuleType7Changed(value);
}

void Preferences::setExtractorRuleType8(int value)
{
    m_extractorRuleTypes[7] = value;
    emit extractorRuleType8Changed(value);
}

void Preferences::setExtractorRuleType9(int value)
{
    m_extractorRuleTypes[8] = value;
    emit extractorRuleType9Changed(value);
}

void Preferences::setExtractorRuleType10(int value)
{
    m_extractorRuleTypes[9] = value;
    emit extractorRuleType10Changed(value);
}

const QString& Preferences::extractorRule1() const
{
    return m_extractorRules[0];
}

const QString& Preferences::extractorRule2() const
{
    return m_extractorRules[1];
}

const QString& Preferences::extractorRule3() const
{
    return m_extractorRules[2];
}

const QString& Preferences::extractorRule4() const
{
    return m_extractorRules[3];
}

const QString& Preferences::extractorRule5() const
{
    return m_extractorRules[4];
}

const QString& Preferences::extractorRule6() const
{
    return m_extractorRules[5];
}

const QString& Preferences::extractorRule7() const
{
    return m_extractorRules[6];
}

const QString& Preferences::extractorRule8() const
{
    return m_extractorRules[7];
}

const QString& Preferences::extractorRule9() const
{
    return m_extractorRules[8];
}

const QString& Preferences::extractorRule10() const
{
    return m_extractorRules[9];
}

void Preferences::setExtractorRule1(const QString& value)
{
    m_extractorRules[0] = value;
    emit extractorRule1Changed(value);
}

void Preferences::setExtractorRule2(const QString& value)
{
    m_extractorRules[1] = value;
    emit extractorRule2Changed(value);
}

void Preferences::setExtractorRule3(const QString& value)
{
    m_extractorRules[2] = value;
    emit extractorRule3Changed(value);
}

void Preferences::setExtractorRule4(const QString& value)
{
    m_extractorRules[3] = value;
    emit extractorRule4Changed(value);
}

void Preferences::setExtractorRule5(const QString& value)
{
    m_extractorRules[4] = value;
    emit extractorRule5Changed(value);
}

void Preferences::setExtractorRule6(const QString& value)
{
    m_extractorRules[5] = value;
    emit extractorRule6Changed(value);
}

void Preferences::setExtractorRule7(const QString& value)
{
    m_extractorRules[6] = value;
    emit extractorRule7Changed(value);
}

void Preferences::setExtractorRule8(const QString& value)
{
    m_extractorRules[7] = value;
    emit extractorRule8Changed(value);
}

void Preferences::setExtractorRule9(const QString& value)
{
    m_extractorRules[8] = value;
    emit extractorRule9Changed(value);
}

void Preferences::setExtractorRule10(const QString& value)
{
    m_extractorRules[9] = value;
    emit extractorRule10Changed(value);
}

int Preferences::extractionType1() const
{
    return m_extractionTypes[0];
}

int Preferences::extractionType2() const
{
    return m_extractionTypes[1];
}

int Preferences::extractionType3() const
{
    return m_extractionTypes[2];
}

int Preferences::extractionType4() const
{
    return m_extractionTypes[3];
}

int Preferences::extractionType5() const
{
    return m_extractionTypes[4];
}

int Preferences::extractionType6() const
{
    return m_extractionTypes[5];
}

int Preferences::extractionType7() const
{
    return m_extractionTypes[6];
}

int Preferences::extractionType8() const
{
    return m_extractionTypes[7];
}

int Preferences::extractionType9() const
{
    return m_extractionTypes[8];
}

int Preferences::extractionType10() const
{
    return m_extractionTypes[9];
}

void Preferences::setExtractionType1(int value)
{
    m_extractionTypes[0] = value;
    emit extractionType1Changed(value);
}

void Preferences::setExtractionType2(int value)
{
    m_extractionTypes[1] = value;
    emit extractionType2Changed(value);
}

void Preferences::setExtractionType3(int value)
{
    m_extractionTypes[2] = value;
    emit extractionType3Changed(value);
}

void Preferences::setExtractionType4(int value)
{
    m_extractionTypes[3] = value;
    emit extractionType4Changed(value);
}

void Preferences::setExtractionType5(int value)
{
    m_extractionTypes[4] = value;
    emit extractionType5Changed(value);
}

void Preferences::setExtractionType6(int value)
{
    m_extractionTypes[5] = value;
    emit extractionType6Changed(value);
}

void Preferences::setExtractionType7(int value)
{
    m_extractionTypes[6] = value;
    emit extractionType7Changed(value);
}

void Preferences::setExtractionType8(int value)
{
    m_extractionTypes[7] = value;
    emit extractionType8Changed(value);
}

void Preferences::setExtractionType9(int value)
{
    m_extractionTypes[8] = value;
    emit extractionType9Changed(value);
}

void Preferences::setExtractionType10(int value)
{
    m_extractionTypes[9] = value;
    emit extractionType10Changed(value);
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

	QMap<QString, QVariant> defaults;

	if (!config.isOpen())
	{
		ERRLOG << "Can't read defaults!";
	}
	else
	{
		defaults = readDefaults(config.readAll());
	}

	QVector<QMetaProperty> metaProperties = allProperties();

	foreach (const auto& metaProperty, metaProperties)
	{
		addDefaultProperty(metaProperty.name(), defaults.contains(metaProperty.name()) ? defaults[metaProperty.name()] : QVariant());
	}
}

QMap<QString, QVariant> Preferences::readDefaults(const QString& str)
{
	const QRegExp rxRecord("^\\s*(((\\w+)/)?(\\w+))\\s*:\\s*([^\\s].{0,})\\b\\s*$");

	const QStringList keyValues = str.split(QRegExp(";\\s*(\\r\\n|\\n)"), QString::SkipEmptyParts);
	QMap<QString, QVariant> defaults;

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
			defaults[rxRecord.cap(1)] = value;
		}
	}

	return defaults;
}

QVector<QMetaProperty> Preferences::allProperties() const
{
	const QMetaObject* thisMetaObject = &Preferences::staticMetaObject;

	QVector<QMetaProperty> metaProperties;
	metaProperties.reserve(thisMetaObject->propertyCount());

	for (int i = 0; i < thisMetaObject->propertyCount(); ++i)
	{
		metaProperties.push_back(thisMetaObject->property(i));
	}

	return metaProperties;
}

}