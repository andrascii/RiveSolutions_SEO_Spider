#pragma once

namespace QuickieWebBot
{

class ISettingsAccessor;

class Preferences : public QObject
{
	Q_OBJECT

	//
	// App properties
	//
	Q_PROPERTY(unsigned threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged);
	Q_PROPERTY(QString applicationLanguage READ applicationLanguage WRITE setApplicationLanguage NOTIFY applicationLanguageChanged);

	//
	// Proxy properties
	//
	Q_PROPERTY(bool useProxy READ useProxy WRITE setUseProxy NOTIFY useProxyChanged);
	Q_PROPERTY(bool proxyNeedAuthorization READ proxyNeedAuthorization WRITE setProxyNeedAuthorization NOTIFY proxyNeedAuthorizationChanged);
	Q_PROPERTY(QString proxyUsername READ proxyUsername WRITE setProxyUsername NOTIFY proxyUsernameChanged);
	Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged);
	Q_PROPERTY(QString proxyAddress READ proxyAddress WRITE setProxyAddress NOTIFY proxyAddressChanged);
	Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged);

	//
	// Web crawler properties
	//
	Q_PROPERTY(bool checkImages READ checkImages WRITE setCheckImages NOTIFY checkImagesChanged);
	Q_PROPERTY(bool checkCSS READ checkCSS WRITE setCheckCSS NOTIFY checkCSSChanged);
	Q_PROPERTY(bool checkJavaScript READ checkJavaScript WRITE setCheckJavaScript NOTIFY checkJavaScriptChanged);
	Q_PROPERTY(bool checkSWF READ checkSWF WRITE setCheckSWF NOTIFY checkSWFChanged);
	Q_PROPERTY(bool checkExternalUrls READ checkExternalUrls WRITE setCheckExternalUrls NOTIFY checkExternalUrlsChanged);
	Q_PROPERTY(bool followInternalNoFollow READ followInternalNoFollow WRITE setFollowInternalNoFollow NOTIFY followInternalNoFollowChanged);
	Q_PROPERTY(bool followExternalNoFollow READ followExternalNoFollow WRITE setFollowExternalNoFollow NOTIFY followExternalNoFollowChanged);
	Q_PROPERTY(bool checkSubdomains READ checkSubdomains WRITE setCheckSubdomains NOTIFY checkSubdomainsChanged);
	Q_PROPERTY(bool checkCanonicals READ checkCanonicals WRITE setCheckCanonicals NOTIFY checkCanonicalsChanged);
	Q_PROPERTY(bool followRobotsTxt READ followRobotsTxt WRITE setFollowRobotsTxt NOTIFY followRobotsTxtChanged);
	Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged);

	//
	// Limits properties
	//
	Q_PROPERTY(int limitSearchTotal READ limitSearchTotal WRITE setLimitSearchTotal NOTIFY limitSearchTotalChanged);
	Q_PROPERTY(int limitMaxUrlLength READ limitMaxUrlLength WRITE setLimitMaxUrlLength NOTIFY limitMaxUrlLengthChanged);
	Q_PROPERTY(int limitTimeout READ limitTimeout WRITE setLimitTimeout NOTIFY limitTimeoutChanged);
	Q_PROPERTY(int maxRedirectCount READ maxRedirectCount WRITE setMaxRedirectCount NOTIFY maxRedirectCountChanged);

	//
	// Preferences
	//
	Q_PROPERTY(int maxH1LengthChars READ maxH1LengthChars WRITE setMaxH1LengthChars NOTIFY maxH1LengthCharsChanged);
	Q_PROPERTY(int maxH2LengthChars READ maxH2LengthChars WRITE setMaxH2LengthChars NOTIFY maxH2LengthCharsChanged);
	Q_PROPERTY(int maxImageAltTextChars READ maxImageAltTextChars WRITE setMaxImageAltTextChars NOTIFY maxImageAltTextCharsChanged);
	Q_PROPERTY(int maxImageSize READ maxImageSize WRITE setMaxImageSize NOTIFY maxImageSizeChanged);
	Q_PROPERTY(int maxDescriptionLength READ maxDescriptionLength WRITE setMaxDescriptionLength NOTIFY maxDescriptionLengthChanged);
	Q_PROPERTY(int minDescriptionLength READ minDescriptionLength WRITE setMinDescriptionLength NOTIFY minDescriptionLengthChanged);
	Q_PROPERTY(int maxTitleLength READ maxTitleLength WRITE setMaxTitleLength NOTIFY maxTitleLengthChanged);
	Q_PROPERTY(int minTitleLength READ minTitleLength WRITE setMinTitleLength NOTIFY minTitleLengthChanged);

	//
	// Crawler start
	//
	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged);

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

	bool followRobotsTxt() const;
	Q_SLOT void setFollowRobotsTxt(bool value);
	Q_SIGNAL void followRobotsTxtChanged();

	const QString& userAgent() const;
	Q_SLOT void setUserAgent(const QString& value);
	Q_SIGNAL void userAgentChanged();

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

	int maxH2LengthChars() const;
	Q_SLOT void setMaxH2LengthChars(int value);
	Q_SIGNAL void maxH2LengthCharsChanged();

	int maxImageAltTextChars() const;
	Q_SLOT void setMaxImageAltTextChars(int value);
	Q_SIGNAL void maxImageAltTextCharsChanged();

	int maxImageSize() const;
	Q_SLOT void setMaxImageSize(int value);
	Q_SIGNAL void maxImageSizeChanged();

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

	const QUrl& url() const;
	Q_SLOT void setUrl(const QUrl& url);
	Q_SIGNAL void urlChanged();

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
	bool m_followRobotsTxt;
	QString m_userAgent;

	int m_limitSearchTotal;
	int m_limitMaxUrlLength;
	int m_limitTimeout;
	int m_maxRedirectCount;

	int m_maxH1LengthChars;
	int m_maxH2LengthChars;
	int m_maxImageAltTextChars;
	int m_maxImageSize;
	int m_maxDescriptionLength;
	int m_minDescriptionLength;
	int m_maxTitleLength;
	int m_minTitleLength;

	QUrl m_url;
};

}
