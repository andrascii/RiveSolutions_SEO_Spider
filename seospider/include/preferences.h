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
	Q_PROPERTY(bool checkExternalUrls READ checkExternalUrls WRITE setCheckExternalUrls NOTIFY checkExternalUrlsChanged);
	Q_PROPERTY(bool followInternalNoFollow READ followInternalNoFollow WRITE setFollowInternalNoFollow NOTIFY followInternalNoFollowChanged);
	Q_PROPERTY(bool followExternalNoFollow READ followExternalNoFollow WRITE setFollowExternalNoFollow NOTIFY followExternalNoFollowChanged);
	Q_PROPERTY(bool checkSubdomains READ checkSubdomains WRITE setCheckSubdomains NOTIFY checkSubdomainsChanged);
	Q_PROPERTY(bool checkCanonicals READ checkCanonicals WRITE setCheckCanonicals NOTIFY checkCanonicalsChanged);
	Q_PROPERTY(bool crawlOutsideOfStartFolder READ crawlOutsideOfStartFolder WRITE setCrawlOutsideOfStartFolder NOTIFY crawlOutsideOfStartFolderChanged);

	// Crawler pause-timer properties
	Q_PROPERTY(bool usePauseTimer READ usePauseTimer WRITE setUsePauseTimer NOTIFY usePauseTimerChanged);
	Q_PROPERTY(int fromPauseTimer READ fromPauseTimer WRITE setFromPauseTimer NOTIFY fromPauseTimerChanged);
	Q_PROPERTY(int toPauseTimer READ toPauseTimer WRITE setToPauseTimer NOTIFY toPauseTimerChanged);

	// Limits properties
	Q_PROPERTY(int limitSearchTotal READ limitSearchTotal WRITE setLimitSearchTotal NOTIFY limitSearchTotalChanged);
	Q_PROPERTY(int limitMaxUrlLength READ limitMaxUrlLength WRITE setLimitMaxUrlLength NOTIFY limitMaxUrlLengthChanged);
	Q_PROPERTY(int limitTimeout READ limitTimeout WRITE setLimitTimeout NOTIFY limitTimeoutChanged);
	Q_PROPERTY(int maxRedirectCount READ maxRedirectCount WRITE setMaxRedirectCount NOTIFY maxRedirectCountChanged);
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

public:

	unsigned threadCount() const;
	Q_SLOT void setThreadCount(unsigned value);
	Q_SIGNAL void threadCountChanged();

	QString applicationLanguage() const;
	Q_SLOT void setApplicationLanguage(const QString& value);
	Q_SIGNAL void applicationLanguageChanged(const QString& newValue);

	bool useProxy() const;
	Q_SLOT void setUseProxy(bool value);
	Q_SIGNAL void useProxyChanged();

	bool proxyNeedAuthorization() const;
	Q_SLOT void setProxyNeedAuthorization(bool value);
	Q_SIGNAL void proxyNeedAuthorizationChanged();

	const QString& proxyUsername() const;
	Q_SLOT void setProxyUsername(const QString& value);
	Q_SIGNAL void proxyUsernameChanged();

	const QString& proxyPassword() const;
	Q_SLOT void setProxyPassword(const QString& value);
	Q_SIGNAL void proxyPasswordChanged();

	const QString& proxyAddress() const;
	Q_SLOT void setProxyAddress(const QString& value);
	Q_SIGNAL void proxyAddressChanged();

	int proxyPort() const;
	Q_SLOT void setProxyPort(int value);
	Q_SIGNAL void proxyPortChanged();

	bool checkImages() const;
	Q_SLOT void setCheckImages(bool value);
	Q_SIGNAL void checkImagesChanged();

	bool checkCSS() const;
	Q_SLOT void setCheckCSS(bool value);
	Q_SIGNAL void checkCSSChanged();

	bool checkJavaScript() const;
	Q_SLOT void setCheckJavaScript(bool value);
	Q_SIGNAL void checkJavaScriptChanged();

	bool checkSWF() const;
	Q_SLOT void setCheckSWF(bool value);
	Q_SIGNAL void checkSWFChanged();

	bool checkExternalUrls() const;
	Q_SLOT void setCheckExternalUrls(bool value);
	Q_SIGNAL void checkExternalUrlsChanged();

	bool followInternalNoFollow() const;
	Q_SLOT void setFollowInternalNoFollow(bool value);
	Q_SIGNAL void followInternalNoFollowChanged();

	bool followExternalNoFollow() const;
	Q_SLOT void setFollowExternalNoFollow(bool value);
	Q_SIGNAL void followExternalNoFollowChanged();

	bool checkSubdomains() const;
	Q_SLOT void setCheckSubdomains(bool value);
	Q_SIGNAL void checkSubdomainsChanged();

	bool checkCanonicals() const;
	Q_SLOT void setCheckCanonicals(bool value);
	Q_SIGNAL void checkCanonicalsChanged();

	bool crawlOutsideOfStartFolder() const;
	Q_SLOT void setCrawlOutsideOfStartFolder(bool value);
	Q_SIGNAL void crawlOutsideOfStartFolderChanged();

	bool usePauseTimer() const;
	Q_SLOT void setUsePauseTimer(bool value);
	Q_SIGNAL void usePauseTimerChanged();

	int fromPauseTimer() const;
	Q_SLOT void setFromPauseTimer(int value);
	Q_SIGNAL void fromPauseTimerChanged();

	int toPauseTimer() const;
	Q_SLOT void setToPauseTimer(int value);
	Q_SIGNAL void toPauseTimerChanged();

	bool followRobotsTxt() const;
	Q_SLOT void setFollowRobotsTxt(bool value);
	Q_SIGNAL void followRobotsTxtChanged();

	int robotSignature() const;
	Q_SLOT void setRobotSignature(const QVariant& value);
	Q_SIGNAL void robotSignatureChanged();

	bool useCustomUserAgent() const;
	Q_SLOT void setUseCustomUserAgent(bool value);
	Q_SIGNAL void useCustomUserAgentChanged();

	bool useDesktopUserAgent() const;
	Q_SLOT void setUseDesktopUserAgent(bool value);
	Q_SIGNAL void useDesktopUserAgentChanged();

	bool useMobileUserAgent() const;
	Q_SLOT void setUseMobileUserAgent(bool value);
	Q_SIGNAL void useMobileUserAgentChanged();

	const QString& desktopUserAgent() const;
	Q_SLOT void setDesktopUserAgent(const QVariant& value);
	Q_SIGNAL void desktopUserAgentChanged();

	const QString& mobileUserAgent() const;
	Q_SLOT void setMobileUserAgent(const QVariant& value);
	Q_SIGNAL void mobileUserAgentChanged();

	int limitSearchTotal() const;
	Q_SLOT void setLimitSearchTotal(int value);
	Q_SIGNAL void limitSearchTotalChanged();

	int limitMaxUrlLength() const;
	Q_SLOT void setLimitMaxUrlLength(int value);
	Q_SIGNAL void limitMaxUrlLengthChanged();

	int limitTimeout() const;
	Q_SLOT void setLimitTimeout(int value);
	Q_SIGNAL void limitTimeoutChanged();

	int maxRedirectCount() const;
	Q_SLOT void setMaxRedirectCount(int value);
	Q_SIGNAL void maxRedirectCountChanged();

	int maxH1LengthChars() const;
	Q_SLOT void setMaxH1LengthChars(int value);
	Q_SIGNAL void maxH1LengthCharsChanged();

	int maxLinksCountOnPage() const;
	Q_SLOT void setMaxLinksCountOnPage(int value);
	Q_SIGNAL void maxLinksCountOnPageChanged();

	int maxH2LengthChars() const;
	Q_SLOT void setMaxH2LengthChars(int value);
	Q_SIGNAL void maxH2LengthCharsChanged();

	int maxImageAltTextChars() const;
	Q_SLOT void setMaxImageAltTextChars(int value);
	Q_SIGNAL void maxImageAltTextCharsChanged();

	int maxImageSize() const;
	Q_SLOT void setMaxImageSize(int value);
	Q_SIGNAL void maxImageSizeChanged();

	int maxPageSize() const;
	Q_SLOT void setMaxPageSize(int value);
	Q_SIGNAL void maxPageSizeChanged();

	int maxDescriptionLength() const;
	Q_SLOT void setMaxDescriptionLength(int value);
	Q_SIGNAL void maxDescriptionLengthChanged();

	int minDescriptionLength() const;
	Q_SLOT void setMinDescriptionLength(int value);
	Q_SIGNAL void minDescriptionLengthChanged();

	int maxTitleLength() const;
	Q_SLOT void setMaxTitleLength(int value);
	Q_SIGNAL void maxTitleLengthChanged();

	int minTitleLength() const;
	Q_SLOT void setMinTitleLength(int value);
	Q_SIGNAL void minTitleLengthChanged();

	QString companyName() const;
	Q_SLOT void setCompanyName(QString value);
	Q_SIGNAL void companyNameChanged();

	QString companyEmail() const;
	Q_SLOT void setCompanyEmail(QString value);
	Q_SIGNAL void companyEmailChanged();

	QString companyWebSite() const;
	Q_SLOT void setCompanyWebSite(QString value);
	Q_SIGNAL void companyWebSiteChanged();

	bool useCustomColorForNotIndexedPages() const;
	Q_SLOT void setUseCustomColorForNotIndexedPages(bool value);
	Q_SIGNAL void useCustomColorForNotIndexedPagesChanged();

	QColor notIndexedPagesColor() const;
	Q_SLOT void setNotIndexedPagesColor(QColor value);
	Q_SIGNAL void notIndexedPagesColorChanged();

	//////////////////////////////////////////////////////////////////////////


	Preferences(ISettingsAccessor* settingsAccessor, QObject* parent = nullptr);
	~Preferences();

	void load();

	enum SettingsGroup
	{
		ProjectSettings,
		General,
		WindowsSettings
	};

	enum UserAgentType
	{
		Desktop,
		Mobile
	};
	
private:
	void readDefaults(const QString& str);
	void addDefaultProperty(const QByteArray& key, const QVariant& defaultValue) noexcept;

private:
	ISettingsAccessor* m_settingsAccessor;
	std::map<QString, QVariant> m_defaults;

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
	bool m_checkExternalUrls;
	bool m_followInternalNoFollow;
	bool m_followExternalNoFollow;
	bool m_checkSubdomains;
	bool m_checkCanonicals;
	bool m_crawlOutsideOfStartFolder;

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
};

}
