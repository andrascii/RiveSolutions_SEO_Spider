#pragma once

namespace SeoSpider
{

class ISettingsAccessor;

class Preferences : public QObject
{
	Q_OBJECT

	// App properties
	Q_PROPERTY(unsigned threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged);
	Q_PROPERTY(QString applicationLanguage READ applicationLanguage WRITE setApplicationLanguage NOTIFY applicationLanguageChanged);

	// Proxy properties
	Q_PROPERTY(bool useProxy READ useProxy WRITE setUseProxy NOTIFY useProxyChanged);
	Q_PROPERTY(bool proxyNeedAuthorization READ proxyNeedAuthorization WRITE setProxyNeedAuthorization NOTIFY proxyNeedAuthorizationChanged);
	Q_PROPERTY(QString proxyUsername READ proxyUsername WRITE setProxyUsername NOTIFY proxyUsernameChanged);
	Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged);
	Q_PROPERTY(QString proxyAddress READ proxyAddress WRITE setProxyAddress NOTIFY proxyAddressChanged);
	Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged);

	// crawler properties
	Q_PROPERTY(bool checkImages READ checkImages WRITE setCheckImages NOTIFY checkImagesChanged);
	Q_PROPERTY(bool checkCSS READ checkCSS WRITE setCheckCSS NOTIFY checkCSSChanged);
	Q_PROPERTY(bool checkJavaScript READ checkJavaScript WRITE setCheckJavaScript NOTIFY checkJavaScriptChanged);
	Q_PROPERTY(bool checkSWF READ checkSWF WRITE setCheckSWF NOTIFY checkSWFChanged);
	Q_PROPERTY(bool checkVideo READ checkVideo WRITE setCheckVideo NOTIFY checkVideoChanged);
	Q_PROPERTY(bool checkOther READ checkOther WRITE setCheckOther NOTIFY checkOtherChanged);
	Q_PROPERTY(bool checkExternalUrls READ checkExternalUrls WRITE setCheckExternalUrls NOTIFY checkExternalUrlsChanged);
	Q_PROPERTY(bool followInternalNoFollow READ followInternalNoFollow WRITE setFollowInternalNoFollow NOTIFY followInternalNoFollowChanged);
	Q_PROPERTY(bool followExternalNoFollow READ followExternalNoFollow WRITE setFollowExternalNoFollow NOTIFY followExternalNoFollowChanged);
	Q_PROPERTY(bool checkSubdomains READ checkSubdomains WRITE setCheckSubdomains NOTIFY checkSubdomainsChanged);
	Q_PROPERTY(bool checkCanonicals READ checkCanonicals WRITE setCheckCanonicals NOTIFY checkCanonicalsChanged);
	Q_PROPERTY(bool crawlOutsideOfStartFolder READ crawlOutsideOfStartFolder WRITE setCrawlOutsideOfStartFolder NOTIFY crawlOutsideOfStartFolderChanged);
	Q_PROPERTY(bool crawlMetaHrefLangLinks READ crawlMetaHrefLangLinks WRITE setCrawlMetaHrefLangLinks NOTIFY crawlMetaHrefLangLinksChanged)
	Q_PROPERTY(QString excludeUrlRegExps READ excludeUrlRegExps WRITE setExcludeUrlRegExps NOTIFY excludeUrlRegExpsChanged)

	// Crawler pause-timer properties
	Q_PROPERTY(bool usePauseTimer READ usePauseTimer WRITE setUsePauseTimer NOTIFY usePauseTimerChanged);
	Q_PROPERTY(int fromPauseTimer READ fromPauseTimer WRITE setFromPauseTimer NOTIFY fromPauseTimerChanged);
	Q_PROPERTY(int toPauseTimer READ toPauseTimer WRITE setToPauseTimer NOTIFY toPauseTimerChanged);

	// Limits properties
	Q_PROPERTY(int limitSearchTotal READ limitSearchTotal WRITE setLimitSearchTotal NOTIFY limitSearchTotalChanged);
	Q_PROPERTY(int limitMaxUrlLength READ limitMaxUrlLength WRITE setLimitMaxUrlLength NOTIFY limitMaxUrlLengthChanged);
	Q_PROPERTY(int limitTimeout READ limitTimeout WRITE setLimitTimeout NOTIFY limitTimeoutChanged);
	Q_PROPERTY(int maxRedirectCount READ maxRedirectCount WRITE setMaxRedirectCount NOTIFY maxRedirectCountChanged);
	Q_PROPERTY(int maxParallelConnections READ maxParallelConnections WRITE setMaxParallelConnections NOTIFY maxParallelConnectionsChanged);
	Q_PROPERTY(int maxLinksCountOnPage READ maxLinksCountOnPage WRITE setMaxLinksCountOnPage NOTIFY maxLinksCountOnPageChanged);

	// User agent properties
	Q_PROPERTY(bool followRobotsTxt READ followRobotsTxt WRITE setFollowRobotsTxt NOTIFY followRobotsTxtChanged);
	Q_PROPERTY(int robotSignature READ robotSignature WRITE setRobotSignature NOTIFY robotSignatureChanged);
	Q_PROPERTY(bool useCustomUserAgent READ useCustomUserAgent WRITE setUseCustomUserAgent NOTIFY useCustomUserAgentChanged);
	Q_PROPERTY(bool useDesktopUserAgent READ useDesktopUserAgent WRITE setUseDesktopUserAgent NOTIFY useDesktopUserAgentChanged);
	Q_PROPERTY(bool useMobileUserAgent READ useMobileUserAgent WRITE setUseMobileUserAgent NOTIFY useMobileUserAgentChanged);
	Q_PROPERTY(QString desktopUserAgent READ desktopUserAgent WRITE setDesktopUserAgent NOTIFY desktopUserAgentChanged);
	Q_PROPERTY(QString mobileUserAgent READ mobileUserAgent WRITE setMobileUserAgent NOTIFY mobileUserAgentChanged);

	// Preferences
	Q_PROPERTY(int maxH1LengthChars READ maxH1LengthChars WRITE setMaxH1LengthChars NOTIFY maxH1LengthCharsChanged);
	Q_PROPERTY(int maxH2LengthChars READ maxH2LengthChars WRITE setMaxH2LengthChars NOTIFY maxH2LengthCharsChanged);
	Q_PROPERTY(int maxImageAltTextChars READ maxImageAltTextChars WRITE setMaxImageAltTextChars NOTIFY maxImageAltTextCharsChanged);
	Q_PROPERTY(int maxImageSize READ maxImageSize WRITE setMaxImageSize NOTIFY maxImageSizeChanged);
	Q_PROPERTY(int maxPageSize READ maxPageSize WRITE setMaxPageSize NOTIFY maxPageSizeChanged);
	Q_PROPERTY(int maxDescriptionLength READ maxDescriptionLength WRITE setMaxDescriptionLength NOTIFY maxDescriptionLengthChanged);
	Q_PROPERTY(int minDescriptionLength READ minDescriptionLength WRITE setMinDescriptionLength NOTIFY minDescriptionLengthChanged);
	Q_PROPERTY(int maxTitleLength READ maxTitleLength WRITE setMaxTitleLength NOTIFY maxTitleLengthChanged);
	Q_PROPERTY(int minTitleLength READ minTitleLength WRITE setMinTitleLength NOTIFY minTitleLengthChanged);

	// Company profile
	Q_PROPERTY(QString companyName READ companyName WRITE setCompanyName NOTIFY companyNameChanged);
	Q_PROPERTY(QString companyEmail READ companyEmail WRITE setCompanyEmail NOTIFY companyEmailChanged);
	Q_PROPERTY(QString companyWebSite READ companyWebSite WRITE setCompanyWebSite NOTIFY companyWebSiteChanged);

	// Page visual properties
	Q_PROPERTY(bool useCustomColorForNotIndexedPages READ useCustomColorForNotIndexedPages WRITE setUseCustomColorForNotIndexedPages NOTIFY useCustomColorForNotIndexedPagesChanged);
	Q_PROPERTY(QColor notIndexedPagesColor READ notIndexedPagesColor WRITE setNotIndexedPagesColor NOTIFY notIndexedPagesColorChanged);

	// Yandex metrica counters
	Q_PROPERTY(bool searchYandexMetricaCounters READ searchYandexMetricaCounters WRITE setSearchYandexMetricaCounters NOTIFY searchYandexMetricaCountersChanged);

	Q_PROPERTY(bool searchYandexMetricaCounter1 READ searchYandexMetricaCounter1 WRITE setSearchYandexMetricaCounter1 NOTIFY searchYandexMetricaCounter1Changed);
	Q_PROPERTY(int yandexMetricaCounter1Id READ yandexMetricaCounter1Id WRITE setYandexMetricaCounter1Id NOTIFY yandexMetricaCounter1IdChanged);

	Q_PROPERTY(bool searchYandexMetricaCounter2 READ searchYandexMetricaCounter2 WRITE setSearchYandexMetricaCounter2 NOTIFY searchYandexMetricaCounter2Changed);
	Q_PROPERTY(int yandexMetricaCounter2Id READ yandexMetricaCounter2Id WRITE setYandexMetricaCounter2Id NOTIFY yandexMetricaCounter2IdChanged);

	Q_PROPERTY(bool searchYandexMetricaCounter3 READ searchYandexMetricaCounter3 WRITE setSearchYandexMetricaCounter3 NOTIFY searchYandexMetricaCounter3Changed);
	Q_PROPERTY(int yandexMetricaCounter3Id READ yandexMetricaCounter3Id WRITE setYandexMetricaCounter3Id NOTIFY yandexMetricaCounter3IdChanged);

	Q_PROPERTY(bool searchYandexMetricaCounter4 READ searchYandexMetricaCounter4 WRITE setSearchYandexMetricaCounter4 NOTIFY searchYandexMetricaCounter4Changed);
	Q_PROPERTY(int yandexMetricaCounter4Id READ yandexMetricaCounter4Id WRITE setYandexMetricaCounter4Id NOTIFY yandexMetricaCounter4IdChanged);

	Q_PROPERTY(bool searchYandexMetricaCounter5 READ searchYandexMetricaCounter5 WRITE setSearchYandexMetricaCounter5 NOTIFY searchYandexMetricaCounter5Changed);
	Q_PROPERTY(int yandexMetricaCounter5Id READ yandexMetricaCounter5Id WRITE setYandexMetricaCounter5Id NOTIFY yandexMetricaCounter5IdChanged);

	// Google Analytics counters
	Q_PROPERTY(bool searchGoogleAnalyticsCounters READ searchGoogleAnalyticsCounters WRITE setSearchGoogleAnalyticsCounters NOTIFY searchGoogleAnalyticsCountersChanged);

	Q_PROPERTY(bool searchGoogleAnalyticsCounter1 READ searchGoogleAnalyticsCounter1 WRITE setSearchGoogleAnalyticsCounter1 NOTIFY searchGoogleAnalyticsCounter1Changed);
	Q_PROPERTY(QString googleAnalyticsCounter1Id READ googleAnalyticsCounter1Id WRITE setGoogleAnalyticsCounter1Id NOTIFY googleAnalyticsCounter1IdChanged);

	Q_PROPERTY(bool searchGoogleAnalyticsCounter2 READ searchGoogleAnalyticsCounter2 WRITE setSearchGoogleAnalyticsCounter2 NOTIFY searchGoogleAnalyticsCounter2Changed);
	Q_PROPERTY(QString googleAnalyticsCounter2Id READ googleAnalyticsCounter2Id WRITE setGoogleAnalyticsCounter2Id NOTIFY googleAnalyticsCounter2IdChanged);

	Q_PROPERTY(bool searchGoogleAnalyticsCounter3 READ searchGoogleAnalyticsCounter3 WRITE setSearchGoogleAnalyticsCounter3 NOTIFY searchGoogleAnalyticsCounter3Changed);
	Q_PROPERTY(QString googleAnalyticsCounter3Id READ googleAnalyticsCounter3Id WRITE setGoogleAnalyticsCounter3Id NOTIFY googleAnalyticsCounter3IdChanged);

	Q_PROPERTY(bool searchGoogleAnalyticsCounter4 READ searchGoogleAnalyticsCounter4 WRITE setSearchGoogleAnalyticsCounter4 NOTIFY searchGoogleAnalyticsCounter4Changed);
	Q_PROPERTY(QString googleAnalyticsCounter4Id READ googleAnalyticsCounter4Id WRITE setGoogleAnalyticsCounter4Id NOTIFY googleAnalyticsCounter4IdChanged);

	Q_PROPERTY(bool searchGoogleAnalyticsCounter5 READ searchGoogleAnalyticsCounter5 WRITE setSearchGoogleAnalyticsCounter5 NOTIFY searchGoogleAnalyticsCounter5Changed);
	Q_PROPERTY(QString googleAnalyticsCounter5Id READ googleAnalyticsCounter5Id WRITE setGoogleAnalyticsCounter5Id NOTIFY googleAnalyticsCounter5IdChanged);

public:

	unsigned threadCount() const;
	Q_SLOT void setThreadCount(unsigned value);
	Q_SIGNAL void threadCountChanged(unsigned value);

	QString applicationLanguage() const;
	Q_SLOT void setApplicationLanguage(const QString& value);
	Q_SIGNAL void applicationLanguageChanged(const QString& newValue);

	bool useProxy() const;
	Q_SLOT void setUseProxy(bool value);
	Q_SIGNAL void useProxyChanged(bool value);

	bool proxyNeedAuthorization() const;
	Q_SLOT void setProxyNeedAuthorization(bool value);
	Q_SIGNAL void proxyNeedAuthorizationChanged(bool value);

	const QString& proxyUsername() const;
	Q_SLOT void setProxyUsername(const QString& value);
	Q_SIGNAL void proxyUsernameChanged(const QString& value);

	const QString& proxyPassword() const;
	Q_SLOT void setProxyPassword(const QString& value);
	Q_SIGNAL void proxyPasswordChanged(const QString& value);

	const QString& proxyAddress() const;
	Q_SLOT void setProxyAddress(const QString& value);
	Q_SIGNAL void proxyAddressChanged(const QString& value);

	int proxyPort() const;
	Q_SLOT void setProxyPort(int value);
	Q_SIGNAL void proxyPortChanged(int value);

	bool checkImages() const;
	Q_SLOT void setCheckImages(bool value);
	Q_SIGNAL void checkImagesChanged(int value);

	bool checkCSS() const;
	Q_SLOT void setCheckCSS(bool value);
	Q_SIGNAL void checkCSSChanged(int value);

	bool checkJavaScript() const;
	Q_SLOT void setCheckJavaScript(bool value);
	Q_SIGNAL void checkJavaScriptChanged(bool value);

	bool checkSWF() const;
	Q_SLOT void setCheckSWF(bool value);
	Q_SIGNAL void checkSWFChanged(bool value);

	bool checkVideo() const;
	Q_SLOT void setCheckVideo(bool value);
	Q_SIGNAL void checkVideoChanged(bool value);

	bool checkOther() const;
	Q_SLOT void setCheckOther(bool value);
	Q_SIGNAL void checkOtherChanged(bool value);

	bool checkExternalUrls() const;
	Q_SLOT void setCheckExternalUrls(bool value);
	Q_SIGNAL void checkExternalUrlsChanged(bool value);

	bool followInternalNoFollow() const;
	Q_SLOT void setFollowInternalNoFollow(bool value);
	Q_SIGNAL void followInternalNoFollowChanged(bool value);

	bool followExternalNoFollow() const;
	Q_SLOT void setFollowExternalNoFollow(bool value);
	Q_SIGNAL void followExternalNoFollowChanged(bool value);

	bool checkSubdomains() const;
	Q_SLOT void setCheckSubdomains(bool value);
	Q_SIGNAL void checkSubdomainsChanged(bool value);

	bool checkCanonicals() const;
	Q_SLOT void setCheckCanonicals(bool value);
	Q_SIGNAL void checkCanonicalsChanged(bool value);

	bool crawlOutsideOfStartFolder() const;
	Q_SLOT void setCrawlOutsideOfStartFolder(bool value);
	Q_SIGNAL void crawlOutsideOfStartFolderChanged(bool value);

	bool crawlMetaHrefLangLinks() const;
	void setCrawlMetaHrefLangLinks(bool value);
	Q_SIGNAL void crawlMetaHrefLangLinksChanged(bool value);

	QString excludeUrlRegExps() const;
	void setExcludeUrlRegExps(const QString& value);
	Q_SIGNAL void excludeUrlRegExpsChanged(QString value);

	bool usePauseTimer() const;
	Q_SLOT void setUsePauseTimer(bool value);
	Q_SIGNAL void usePauseTimerChanged(bool value);

	int fromPauseTimer() const;
	Q_SLOT void setFromPauseTimer(int value);
	Q_SIGNAL void fromPauseTimerChanged(int value);

	int toPauseTimer() const;
	Q_SLOT void setToPauseTimer(int value);
	Q_SIGNAL void toPauseTimerChanged(int value);

	bool followRobotsTxt() const;
	Q_SLOT void setFollowRobotsTxt(bool value);
	Q_SIGNAL void followRobotsTxtChanged(bool value);

	int robotSignature() const;
	Q_SLOT void setRobotSignature(const QVariant& value);
	Q_SIGNAL void robotSignatureChanged(const QVariant& value);

	bool useCustomUserAgent() const;
	Q_SLOT void setUseCustomUserAgent(bool value);
	Q_SIGNAL void useCustomUserAgentChanged(bool value);

	bool useDesktopUserAgent() const;
	Q_SLOT void setUseDesktopUserAgent(bool value);
	Q_SIGNAL void useDesktopUserAgentChanged(bool value);

	bool useMobileUserAgent() const;
	Q_SLOT void setUseMobileUserAgent(bool value);
	Q_SIGNAL void useMobileUserAgentChanged(bool value);

	const QString& desktopUserAgent() const;
	Q_SLOT void setDesktopUserAgent(const QVariant& value);
	Q_SIGNAL void desktopUserAgentChanged(const QVariant& value);

	const QString& mobileUserAgent() const;
	Q_SLOT void setMobileUserAgent(const QVariant& value);
	Q_SIGNAL void mobileUserAgentChanged(const QVariant& value);

	int limitSearchTotal() const;
	Q_SLOT void setLimitSearchTotal(int value);
	Q_SIGNAL void limitSearchTotalChanged(int value);

	int limitMaxUrlLength() const;
	Q_SLOT void setLimitMaxUrlLength(int value);
	Q_SIGNAL void limitMaxUrlLengthChanged(int value);

	int limitTimeout() const;
	Q_SLOT void setLimitTimeout(int value);
	Q_SIGNAL void limitTimeoutChanged(int value);

	int maxRedirectCount() const;
	Q_SLOT void setMaxRedirectCount(int value);
	Q_SIGNAL void maxRedirectCountChanged(int value);

	int maxParallelConnections() const;
	Q_SLOT void setMaxParallelConnections(int value);
	Q_SIGNAL void maxParallelConnectionsChanged(int value);

	int maxH1LengthChars() const;
	Q_SLOT void setMaxH1LengthChars(int value);
	Q_SIGNAL void maxH1LengthCharsChanged(int value);

	int maxLinksCountOnPage() const;
	Q_SLOT void setMaxLinksCountOnPage(int value);
	Q_SIGNAL void maxLinksCountOnPageChanged(int value);

	int maxH2LengthChars() const;
	Q_SLOT void setMaxH2LengthChars(int value);
	Q_SIGNAL void maxH2LengthCharsChanged(int value);

	int maxImageAltTextChars() const;
	Q_SLOT void setMaxImageAltTextChars(int value);
	Q_SIGNAL void maxImageAltTextCharsChanged(int value);

	int maxImageSize() const;
	Q_SLOT void setMaxImageSize(int value);
	Q_SIGNAL void maxImageSizeChanged(int value);

	int maxPageSize() const;
	Q_SLOT void setMaxPageSize(int value);
	Q_SIGNAL void maxPageSizeChanged(int value);

	int maxDescriptionLength() const;
	Q_SLOT void setMaxDescriptionLength(int value);
	Q_SIGNAL void maxDescriptionLengthChanged(int value);

	int minDescriptionLength() const;
	Q_SLOT void setMinDescriptionLength(int value);
	Q_SIGNAL void minDescriptionLengthChanged(int value);

	int maxTitleLength() const;
	Q_SLOT void setMaxTitleLength(int value);
	Q_SIGNAL void maxTitleLengthChanged(int value);

	int minTitleLength() const;
	Q_SLOT void setMinTitleLength(int value);
	Q_SIGNAL void minTitleLengthChanged(int value);

	QString companyName() const;
	Q_SLOT void setCompanyName(QString value);
	Q_SIGNAL void companyNameChanged(QString value);

	QString companyEmail() const;
	Q_SLOT void setCompanyEmail(QString value);
	Q_SIGNAL void companyEmailChanged(QString value);

	QString companyWebSite() const;
	Q_SLOT void setCompanyWebSite(QString value);
	Q_SIGNAL void companyWebSiteChanged(QString value);

	bool useCustomColorForNotIndexedPages() const;
	Q_SLOT void setUseCustomColorForNotIndexedPages(bool value);
	Q_SIGNAL void useCustomColorForNotIndexedPagesChanged(bool value);

	QColor notIndexedPagesColor() const;
	Q_SLOT void setNotIndexedPagesColor(QColor value);
	Q_SIGNAL void notIndexedPagesColorChanged(QColor value);


	bool searchYandexMetricaCounters() const;
	Q_SLOT void setSearchYandexMetricaCounters(bool value);
	Q_SIGNAL void searchYandexMetricaCountersChanged(bool value);

	bool searchYandexMetricaCounter1() const;
	Q_SLOT void setSearchYandexMetricaCounter1(bool value);
	Q_SIGNAL void searchYandexMetricaCounter1Changed(bool value);
	int yandexMetricaCounter1Id() const;
	Q_SLOT void setYandexMetricaCounter1Id(int value);
	Q_SIGNAL void yandexMetricaCounter1IdChanged(int value);

	bool searchYandexMetricaCounter2() const;
	Q_SLOT void setSearchYandexMetricaCounter2(bool value);
	Q_SIGNAL void searchYandexMetricaCounter2Changed(bool value);
	int yandexMetricaCounter2Id() const;
	Q_SLOT void setYandexMetricaCounter2Id(int value);
	Q_SIGNAL void yandexMetricaCounter2IdChanged(int value);

	bool searchYandexMetricaCounter3() const;
	Q_SLOT void setSearchYandexMetricaCounter3(bool value);
	Q_SIGNAL void searchYandexMetricaCounter3Changed(bool value);
	int yandexMetricaCounter3Id() const;
	Q_SLOT void setYandexMetricaCounter3Id(int value);
	Q_SIGNAL void yandexMetricaCounter3IdChanged(int value);

	bool searchYandexMetricaCounter4() const;
	Q_SLOT void setSearchYandexMetricaCounter4(bool value);
	Q_SIGNAL void searchYandexMetricaCounter4Changed(bool value);
	int yandexMetricaCounter4Id() const;
	Q_SLOT void setYandexMetricaCounter4Id(int value);
	Q_SIGNAL void yandexMetricaCounter4IdChanged(int value);

	bool searchYandexMetricaCounter5() const;
	Q_SLOT void setSearchYandexMetricaCounter5(bool value);
	Q_SIGNAL void searchYandexMetricaCounter5Changed(bool value);
	int yandexMetricaCounter5Id() const;
	Q_SLOT void setYandexMetricaCounter5Id(int value);
	Q_SIGNAL void yandexMetricaCounter5IdChanged(int value);


	bool searchGoogleAnalyticsCounters() const;
	Q_SLOT void setSearchGoogleAnalyticsCounters(bool value);
	Q_SIGNAL void searchGoogleAnalyticsCountersChanged(bool value);

	bool searchGoogleAnalyticsCounter1() const;
	Q_SLOT void setSearchGoogleAnalyticsCounter1(bool value);
	Q_SIGNAL void searchGoogleAnalyticsCounter1Changed(bool value);
	const QString& googleAnalyticsCounter1Id() const;
	Q_SLOT void setGoogleAnalyticsCounter1Id(const QString& value);
	Q_SIGNAL void googleAnalyticsCounter1IdChanged(const QString& value);

	bool searchGoogleAnalyticsCounter2() const;
	Q_SLOT void setSearchGoogleAnalyticsCounter2(bool value);
	Q_SIGNAL void searchGoogleAnalyticsCounter2Changed(bool value);
	const QString& googleAnalyticsCounter2Id() const;
	Q_SLOT void setGoogleAnalyticsCounter2Id(const QString& value);
	Q_SIGNAL void googleAnalyticsCounter2IdChanged(const QString& value);

	bool searchGoogleAnalyticsCounter3() const;
	Q_SLOT void setSearchGoogleAnalyticsCounter3(bool value);
	Q_SIGNAL void searchGoogleAnalyticsCounter3Changed(bool value);
	const QString& googleAnalyticsCounter3Id() const;
	Q_SLOT void setGoogleAnalyticsCounter3Id(const QString& value);
	Q_SIGNAL void googleAnalyticsCounter3IdChanged(const QString& value);

	bool searchGoogleAnalyticsCounter4() const;
	Q_SLOT void setSearchGoogleAnalyticsCounter4(bool value);
	Q_SIGNAL void searchGoogleAnalyticsCounter4Changed(bool value);
	const QString& googleAnalyticsCounter4Id() const;
	Q_SLOT void setGoogleAnalyticsCounter4Id(const QString& value);
	Q_SIGNAL void googleAnalyticsCounter4IdChanged(const QString& value);

	bool searchGoogleAnalyticsCounter5() const;
	Q_SLOT void setSearchGoogleAnalyticsCounter5(bool value);
	Q_SIGNAL void searchGoogleAnalyticsCounter5Changed(bool value);
	const QString& googleAnalyticsCounter5Id() const;
	Q_SLOT void setGoogleAnalyticsCounter5Id(const QString& value);
	Q_SIGNAL void googleAnalyticsCounter5IdChanged(const QString& value);

public:
	Preferences(ISettingsAccessor* settingsAccessor, QObject* parent = nullptr);
	~Preferences();

	static QString localeFromString(const QString& language);

	void load();

private:
	QMap<QString, QVariant> readDefaults(const QString& str);
	QVector<QMetaProperty> allProperties() const;
	void addDefaultProperty(const QByteArray& key, const QVariant& defaultValue) noexcept;

private:
	ISettingsAccessor* m_settingsAccessor;

	unsigned m_threadCount;
	QString m_applicationLanguage;

	bool m_useProxy;
	bool m_proxyNeedAuthorization;
	QString m_proxyUsername;
	QString m_proxyPassword;
	QString m_proxyAddress;
	int m_proxyPort;

	bool m_checkImages;
	bool m_checkCSS;
	bool m_checkJavaScript;
	bool m_checkSWF;
	bool m_checkVideo;
	bool m_checkOther;
	bool m_checkExternalUrls;
	bool m_followInternalNoFollow;
	bool m_followExternalNoFollow;
	bool m_checkSubdomains;
	bool m_checkCanonicals;
	bool m_crawlOutsideOfStartFolder;
	bool m_crawlMetaHrefLangLinks;
	QString m_excludedUrlRegExps;

	bool m_usePauseTimer;
	int m_fromPauseTimer;
	int m_toPauseTimer;

	bool m_followRobotsTxt;
	int m_robotSignature;
	bool m_useCustomUserAgent;
	bool m_useDesktopUserAgent;
	bool m_useMobileUserAgent;
	QString m_desktopUserAgent;
	QString m_mobileUserAgent;

	int m_limitSearchTotal;
	int m_limitMaxUrlLength;
	int m_limitTimeout;
	int m_maxRedirectCount;
	int m_maxParallelConnections;
	int m_maxLinksCountOnPage;

	int m_maxH1LengthChars;
	int m_maxH2LengthChars;
	int m_maxImageAltTextChars;
	int m_maxImageSize;
	int m_maxPageSize;
	int m_maxDescriptionLength;
	int m_minDescriptionLength;
	int m_maxTitleLength;
	int m_minTitleLength;

	QString m_companyName;
	QString m_companyEmail;
	QString m_companyWebSite;

	bool m_useCustomColorForNotIndexedPages;
	QColor m_notIndexedPagesColor;

	// Yandex metrica
	bool m_searchYandexMetricaCounters;

	bool m_searchYandexMetricaCounter1;
	int m_yandexMetricaCounter1Id;

	bool m_searchYandexMetricaCounter2;
	int m_yandexMetricaCounter2Id;

	bool m_searchYandexMetricaCounter3;
	int m_yandexMetricaCounter3Id;

	bool m_searchYandexMetricaCounter4;
	int m_yandexMetricaCounter4Id;

	bool m_searchYandexMetricaCounter5;
	int m_yandexMetricaCounter5Id;

	// Google Analytics
	bool m_searchGoogleAnalyticsCounters;

	bool m_searchGoogleAnalyticsCounter1;
	QString m_googleAnalyticsCounter1Id;

	bool m_searchGoogleAnalyticsCounter2;
	QString m_googleAnalyticsCounter2Id;

	bool m_searchGoogleAnalyticsCounter3;
	QString m_googleAnalyticsCounter3Id;

	bool m_searchGoogleAnalyticsCounter4;
	QString m_googleAnalyticsCounter4Id;

	bool m_searchGoogleAnalyticsCounter5;
	QString m_googleAnalyticsCounter5Id;
};

}
