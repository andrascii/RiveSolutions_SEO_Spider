#pragma once

namespace QuickieWebBot
{

class ApplicationProperties : public QObject
{
	Q_OBJECT

	//
	// App properties
	//
	Q_PROPERTY(unsigned threadCount READ threadCount WRITE setThreadCount NOTIFY threadCountChanged);

	//
	// Web crawler properties
	//
	Q_PROPERTY(bool useProxy READ useProxy WRITE setUseProxy NOTIFY useProxyChanged);
	Q_PROPERTY(bool checkImages READ checkImages WRITE setCheckImages NOTIFY checkImagesChanged);
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
	Q_PROPERTY(int maxDescriptionWidth READ maxDescriptionWidth WRITE setMaxDescriptionWidth NOTIFY maxDescriptionWidthChanged);
	Q_PROPERTY(int maxTitleWidth READ maxTitleWidth WRITE setMaxTitleWidth NOTIFY maxTitleWidthChanged);

public:
	unsigned threadCount() const;
	Q_SLOT void setThreadCount(unsigned value);
	Q_SIGNAL void threadCountChanged();

	bool useProxy() const;
	Q_SLOT void setUseProxy(bool value);
	Q_SIGNAL void useProxyChanged();

	bool checkImages() const;
	Q_SLOT void setCheckImages(bool value);
	Q_SIGNAL void checkImagesChanged();

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

	int maxDescriptionWidth() const;
	Q_SLOT void setMaxDescriptionWidth(int value);
	Q_SIGNAL void maxDescriptionWidthChanged();
	
	int maxTitleWidth() const;
	Q_SLOT void setMaxTitleWidth(int value);
	Q_SIGNAL void maxTitleWidthChanged();

	//////////////////////////////////////////////////////////////////////////

	ApplicationProperties(QObject* parent);

private:
	unsigned m_threadCount;
	bool m_useProxy;
	bool m_checkImages;
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
	int m_maxDescriptionWidth;
	int m_maxTitleWidth;
};

}
