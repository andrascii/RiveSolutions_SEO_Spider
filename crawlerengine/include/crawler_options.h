#pragma once

#include "icrawler_options.h"
#include "user_agent_type.h"

namespace CrawlerEngine
{

struct CrawlerOptionsData final
{
	Url startCrawlingPage;
	int limitMaxUrlLength = int();
	int limitSearchTotal = int();
	int limitTimeout = int();
	int maxRedirectsToFollow = int();
	int maxParallelConnections = int();
	int maxLinksCountOnPage = int();
	int maxTitleLength = int();
	int minTitleLength = int();
	int maxDescriptionLength = int();
	int minDescriptionLength = int();
	int maxH1LengthChars = int();
	int maxH2LengthChars = int();
	int maxImageAltTextChars = int();
	int maxImageSizeKb = int();
	int maxPageSizeKb = int();
	bool useProxy = bool();
	QString proxyHostName;
	int proxyPort = int();
	QString proxyUser;
	QString proxyPassword;
	bool checkExternalLinks = bool();
	bool followInternalNofollow = bool();
	bool followExternalNofollow = bool();
	bool checkCanonicals = bool();
	bool checkSubdomains = bool();
	bool crawlOutsideOfStartFolder = bool();
	bool crawlMetaHrefLangLinks = bool();
	bool followRobotsTxtRules = bool();
	UserAgentType userAgentToFollow = UserAgentType::AnyBot;
	ParserTypeFlags parserTypeFlags;
	int pauseRangeFrom = -1;
	int pauseRangeTo = -1;
	bool pauseRangeEnabled = false;
	QByteArray userAgent;
	QString excludeUrlRegExps;

	// ym
	bool searchYandexMetricaCounters;

	bool searchYandexMetricaCounter1;
	int yandexMetricaCounter1Id;

	bool searchYandexMetricaCounter2;
	int yandexMetricaCounter2Id;

	bool searchYandexMetricaCounter3;
	int yandexMetricaCounter3Id;

	bool searchYandexMetricaCounter4;
	int yandexMetricaCounter4Id;

	bool searchYandexMetricaCounter5;
	int yandexMetricaCounter5Id;

	// ga
	bool searchGoogleAnalyticsCounters;

	bool searchGoogleAnalyticsCounter1;
	QString googleAnalyticsCounter1Id;

	bool searchGoogleAnalyticsCounter2;
	QString googleAnalyticsCounter2Id;

	bool searchGoogleAnalyticsCounter3;
	QString googleAnalyticsCounter3Id;

	bool searchGoogleAnalyticsCounter4;
	QString googleAnalyticsCounter4Id;

	bool searchGoogleAnalyticsCounter5;
	QString googleAnalyticsCounter5Id;

    // Data Extraction
    QString extractorName1;
    QString extractorName2;
    QString extractorName3;
    QString extractorName4;
    QString extractorName5;
    QString extractorName6;
    QString extractorName7;
    QString extractorName8;
    QString extractorName9;
    QString extractorName10;

    // -1 means that this extractor is disabled
    int extractorRuleType1 = -1;
    int extractorRuleType2 = -1;
    int extractorRuleType3 = -1;
    int extractorRuleType4 = -1;
    int extractorRuleType5 = -1;
    int extractorRuleType6 = -1;
    int extractorRuleType7 = -1;
    int extractorRuleType8 = -1;
    int extractorRuleType9 = -1;
    int extractorRuleType10 = -1;

    QString extractorRule1;
    QString extractorRule2;
    QString extractorRule3;
    QString extractorRule4;
    QString extractorRule5;
    QString extractorRule6;
    QString extractorRule7;
    QString extractorRule8;
    QString extractorRule9;
    QString extractorRule10;

    // -1 means that this extractor is disabled
    int extractionType1 = -1;
    int extractionType2 = -1;
    int extractionType3 = -1;
    int extractionType4 = -1;
    int extractionType5 = -1;
    int extractionType6 = -1;
    int extractionType7 = -1;
    int extractionType8 = -1;
    int extractionType9 = -1;
    int extractionType10 = -1;
};

class CrawlerOptions : public QObject, public ICrawlerOptions
{
    Q_OBJECT

    Q_PROPERTY(Url startCrawlingPage READ startCrawlingPage WRITE setStartCrawlingPage NOTIFY startCrawlingPageChanged);
	Q_PROPERTY(int limitMaxUrlLength READ limitMaxUrlLength WRITE setLimitMaxUrlLength NOTIFY limitMaxUrlLengthChanged);
	Q_PROPERTY(int limitSearchTotal READ limitSearchTotal WRITE setLimitSearchTotal NOTIFY limitSearchTotalChanged);
	Q_PROPERTY(int limitTimeout READ limitTimeout WRITE setLimitTimeout NOTIFY limitTimeoutChanged);
	Q_PROPERTY(int maxRedirectsToFollow READ maxRedirectsToFollow WRITE setMaxRedirectsToFollow NOTIFY maxRedirectsToFollowChanged);
	Q_PROPERTY(int maxParallelConnections READ maxParallelConnections WRITE setMaxParallelConnections NOTIFY maxParallelConnectionsChanged);
	Q_PROPERTY(int maxLinksCountOnPage READ maxLinksCountOnPage WRITE setMaxLinksCountOnPage NOTIFY maxLinksCountOnPageChanged);
	Q_PROPERTY(int maxTitleLength READ maxTitleLength WRITE setMaxTitleLength NOTIFY maxTitleLengthChanged);
	Q_PROPERTY(int minTitleLength READ minTitleLength WRITE setMinTitleLength NOTIFY minTitleLengthChanged);
	Q_PROPERTY(int maxDescriptionLength READ maxDescriptionLength WRITE setMaxDescriptionLength NOTIFY maxDescriptionLengthChanged);
	Q_PROPERTY(int minDescriptionLength READ minDescriptionLength WRITE setMinDescriptionLength NOTIFY minDescriptionLengthChanged);
	Q_PROPERTY(int maxH1LengthChars READ maxH1LengthChars WRITE setMaxH1LengthChars NOTIFY maxH1LengthCharsChanged);
	Q_PROPERTY(int maxH2LengthChars READ maxH2LengthChars WRITE setMaxH2LengthChars NOTIFY maxH2LengthCharsChanged);
	Q_PROPERTY(int maxImageAltTextChars READ maxImageAltTextChars WRITE setMaxImageAltTextChars NOTIFY maxImageAltTextCharsChanged);
	Q_PROPERTY(int maxImageSizeKb READ maxImageSizeKb WRITE setMaxImageSizeKb NOTIFY maxImageSizeKbChanged);
	Q_PROPERTY(int maxPageSizeKb READ maxPageSizeKb WRITE setMaxPageSizeKb NOTIFY maxPageSizeKbChanged);
	Q_PROPERTY(bool useProxy READ useProxy WRITE setUseProxy NOTIFY useProxyChanged);
	Q_PROPERTY(QString proxyHostName READ proxyHostName WRITE setProxyHostName NOTIFY proxyHostNameChanged);
	Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged);
	Q_PROPERTY(QString proxyUser READ proxyUser WRITE setProxyUser NOTIFY proxyUserChanged);
	Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged);
	Q_PROPERTY(bool checkExternalLinks READ checkExternalLinks WRITE setCheckExternalLinks NOTIFY checkExternalLinksChanged);
	Q_PROPERTY(bool followInternalNofollow READ followInternalNofollow WRITE setFollowInternalNofollow NOTIFY followInternalNofollowChanged);
	Q_PROPERTY(bool followExternalNofollow READ followExternalNofollow WRITE setFollowExternalNofollow NOTIFY followExternalNofollowChanged);
	Q_PROPERTY(bool checkCanonicals READ checkCanonicals WRITE setCheckCanonicals NOTIFY checkCanonicalsChanged);
	Q_PROPERTY(bool checkSubdomains READ checkSubdomains WRITE setCheckSubdomains NOTIFY checkSubdomainsChanged);
	Q_PROPERTY(bool crawlOutsideOfStartFolder READ crawlOutsideOfStartFolder WRITE setCrawlOutsideOfStartFolder NOTIFY crawlOutsideOfStartFolderChanged);
	Q_PROPERTY(bool crawlMetaHrefLangLinks READ crawlMetaHrefLangLinks WRITE setCrawlMetaHrefLangLinks NOTIFY crawlMetaHrefLangLinksChanged);
	Q_PROPERTY(bool followRobotsTxtRules READ followRobotsTxtRules WRITE setFollowRobotsTxtRules NOTIFY followRobotsTxtRulesChanged);
	Q_PROPERTY(UserAgentType userAgentToFollow READ userAgentToFollow WRITE setUserAgentToFollow NOTIFY userAgentToFollowChanged);
	Q_PROPERTY(ParserTypeFlags parserTypeFlags READ parserTypeFlags WRITE setParserTypeFlags NOTIFY parserTypeFlagsChanged);
	Q_PROPERTY(int pauseRangeFrom READ pauseRangeFrom WRITE setPauseRangeFrom NOTIFY pauseRangeFromChanged);
	Q_PROPERTY(int pauseRangeTo READ pauseRangeTo WRITE setPauseRangeTo NOTIFY pauseRangeToChanged);
	Q_PROPERTY(bool pauseRangeEnabled READ pauseRangeEnabled WRITE setPauseRangeEnabled NOTIFY pauseRangeEnabledChanged);
	Q_PROPERTY(QByteArray userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged);
	Q_PROPERTY(QString excludeUrlRegExps READ excludeUrlRegExps WRITE setExcludeUrlRegExps NOTIFY excludeUrlRegExpsChanged);

	// yandex metrica
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

	// google analytics
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

    // Data Extraction
    Q_PROPERTY(QString extractorName1 READ extractorName1 WRITE setExtractorName1 NOTIFY extractorName1Changed);
    Q_PROPERTY(QString extractorName2 READ extractorName2 WRITE setExtractorName2 NOTIFY extractorName2Changed);
    Q_PROPERTY(QString extractorName3 READ extractorName3 WRITE setExtractorName3 NOTIFY extractorName3Changed);
    Q_PROPERTY(QString extractorName4 READ extractorName4 WRITE setExtractorName4 NOTIFY extractorName4Changed);
    Q_PROPERTY(QString extractorName5 READ extractorName5 WRITE setExtractorName5 NOTIFY extractorName5Changed);
    Q_PROPERTY(QString extractorName6 READ extractorName6 WRITE setExtractorName6 NOTIFY extractorName6Changed);
    Q_PROPERTY(QString extractorName7 READ extractorName7 WRITE setExtractorName7 NOTIFY extractorName7Changed);
    Q_PROPERTY(QString extractorName8 READ extractorName8 WRITE setExtractorName8 NOTIFY extractorName8Changed);
    Q_PROPERTY(QString extractorName9 READ extractorName9 WRITE setExtractorName9 NOTIFY extractorName9Changed);
    Q_PROPERTY(QString extractorName10 READ extractorName10 WRITE setExtractorName10 NOTIFY extractorName10Changed);

    Q_PROPERTY(int extractorRuleType1 READ extractorRuleType1 WRITE setExtractorRuleType1 NOTIFY extractorRuleType1Changed);
    Q_PROPERTY(int extractorRuleType2 READ extractorRuleType2 WRITE setExtractorRuleType2 NOTIFY extractorRuleType2Changed);
    Q_PROPERTY(int extractorRuleType3 READ extractorRuleType3 WRITE setExtractorRuleType3 NOTIFY extractorRuleType3Changed);
    Q_PROPERTY(int extractorRuleType4 READ extractorRuleType4 WRITE setExtractorRuleType4 NOTIFY extractorRuleType4Changed);
    Q_PROPERTY(int extractorRuleType5 READ extractorRuleType5 WRITE setExtractorRuleType5 NOTIFY extractorRuleType5Changed);
    Q_PROPERTY(int extractorRuleType6 READ extractorRuleType6 WRITE setExtractorRuleType6 NOTIFY extractorRuleType6Changed);
    Q_PROPERTY(int extractorRuleType7 READ extractorRuleType7 WRITE setExtractorRuleType7 NOTIFY extractorRuleType7Changed);
    Q_PROPERTY(int extractorRuleType8 READ extractorRuleType8 WRITE setExtractorRuleType8 NOTIFY extractorRuleType8Changed);
    Q_PROPERTY(int extractorRuleType9 READ extractorRuleType9 WRITE setExtractorRuleType9 NOTIFY extractorRuleType9Changed);
    Q_PROPERTY(int extractorRuleType10 READ extractorRuleType10 WRITE setExtractorRuleType10 NOTIFY extractorRuleType10Changed);

    Q_PROPERTY(QString extractorRule1 READ extractorRule1 WRITE setExtractorRule1 NOTIFY extractorRule1Changed);
    Q_PROPERTY(QString extractorRule2 READ extractorRule2 WRITE setExtractorRule2 NOTIFY extractorRule2Changed);
    Q_PROPERTY(QString extractorRule3 READ extractorRule3 WRITE setExtractorRule3 NOTIFY extractorRule3Changed);
    Q_PROPERTY(QString extractorRule4 READ extractorRule4 WRITE setExtractorRule4 NOTIFY extractorRule4Changed);
    Q_PROPERTY(QString extractorRule5 READ extractorRule5 WRITE setExtractorRule5 NOTIFY extractorRule5Changed);
    Q_PROPERTY(QString extractorRule6 READ extractorRule6 WRITE setExtractorRule6 NOTIFY extractorRule6Changed);
    Q_PROPERTY(QString extractorRule7 READ extractorRule7 WRITE setExtractorRule7 NOTIFY extractorRule7Changed);
    Q_PROPERTY(QString extractorRule8 READ extractorRule8 WRITE setExtractorRule8 NOTIFY extractorRule8Changed);
    Q_PROPERTY(QString extractorRule9 READ extractorRule9 WRITE setExtractorRule9 NOTIFY extractorRule9Changed);
    Q_PROPERTY(QString extractorRule10 READ extractorRule10 WRITE setExtractorRule10 NOTIFY extractorRule10Changed);

    Q_PROPERTY(int extractionType1 READ extractionType1 WRITE setExtractionType1 NOTIFY extractionType1Changed);
    Q_PROPERTY(int extractionType2 READ extractionType2 WRITE setExtractionType2 NOTIFY extractionType2Changed);
    Q_PROPERTY(int extractionType3 READ extractionType3 WRITE setExtractionType3 NOTIFY extractionType3Changed);
    Q_PROPERTY(int extractionType4 READ extractionType4 WRITE setExtractionType4 NOTIFY extractionType4Changed);
    Q_PROPERTY(int extractionType5 READ extractionType5 WRITE setExtractionType5 NOTIFY extractionType5Changed);
    Q_PROPERTY(int extractionType6 READ extractionType6 WRITE setExtractionType6 NOTIFY extractionType6Changed);
    Q_PROPERTY(int extractionType7 READ extractionType7 WRITE setExtractionType7 NOTIFY extractionType7Changed);
    Q_PROPERTY(int extractionType8 READ extractionType8 WRITE setExtractionType8 NOTIFY extractionType8Changed);
    Q_PROPERTY(int extractionType9 READ extractionType9 WRITE setExtractionType9 NOTIFY extractionType9Changed);
    Q_PROPERTY(int extractionType10 READ extractionType10 WRITE setExtractionType10 NOTIFY extractionType10Changed);

public:
	CrawlerOptions(QObject* parent = nullptr);

	virtual QObject* qobject() const noexcept override;

	virtual const CrawlerOptionsData& data() const noexcept override;
	virtual void setData(const CrawlerOptionsData& data) noexcept override;
	virtual void setData(CrawlerOptionsData&& data) noexcept override;
	Q_SIGNAL virtual void dataChanged(const CrawlerOptionsData& data) const override;

	virtual Url startCrawlingPage() const noexcept override;
	Q_SLOT virtual void setStartCrawlingPage(const Url& url) override;
	Q_SIGNAL virtual void startCrawlingPageChanged(const Url& url) const override;

	virtual int limitMaxUrlLength() const noexcept override;
	Q_SLOT virtual void setLimitMaxUrlLength(int value) noexcept override;
	Q_SIGNAL virtual void limitMaxUrlLengthChanged(int value) const override;

	virtual int limitSearchTotal() const noexcept override;
	Q_SLOT virtual void setLimitSearchTotal(int value) noexcept override;
	Q_SIGNAL virtual void limitSearchTotalChanged(int value) const override;

	virtual int limitTimeout() const noexcept override;
	Q_SLOT virtual void setLimitTimeout(int value) noexcept override;
	Q_SIGNAL virtual int limitTimeoutChanged(int value) const override;

	virtual int maxRedirectsToFollow() const noexcept override;
	Q_SLOT virtual void setMaxRedirectsToFollow(int value) noexcept override;
	Q_SIGNAL virtual void maxRedirectsToFollowChanged(int value) const override;

	virtual int maxParallelConnections() const noexcept override;
	Q_SLOT virtual void setMaxParallelConnections(int value) noexcept override;
	Q_SIGNAL virtual void maxParallelConnectionsChanged(int value) const override;

	virtual int maxLinksCountOnPage() const noexcept override;
	Q_SLOT virtual void setMaxLinksCountOnPage(int value) noexcept override;
	Q_SIGNAL virtual void maxLinksCountOnPageChanged(int value) const override;

	virtual int maxTitleLength() const noexcept override;
	Q_SLOT virtual void setMaxTitleLength(int value) noexcept override;
	Q_SIGNAL virtual void maxTitleLengthChanged(int value) const override;

	virtual int minTitleLength() const noexcept override;
	Q_SLOT virtual void setMinTitleLength(int value) noexcept override;
	Q_SIGNAL virtual void minTitleLengthChanged(int value) const override;

	virtual int maxDescriptionLength() const noexcept override;
	Q_SLOT virtual void setMaxDescriptionLength(int value) noexcept override;
	Q_SIGNAL virtual void maxDescriptionLengthChanged(int value) const override;

	virtual int minDescriptionLength() const noexcept override;
	Q_SLOT virtual void setMinDescriptionLength(int value) noexcept override;
	Q_SIGNAL virtual void minDescriptionLengthChanged(int value) const override;

	virtual int maxH1LengthChars() const noexcept override;
	Q_SLOT virtual void setMaxH1LengthChars(int value) noexcept override;
	Q_SIGNAL virtual void maxH1LengthCharsChanged(int value) const override;

	virtual int maxH2LengthChars() const noexcept override;
	Q_SLOT virtual void setMaxH2LengthChars(int value) noexcept override;
	Q_SIGNAL virtual void maxH2LengthCharsChanged(int value) const override;

	virtual int maxImageAltTextChars() const noexcept override;
	Q_SLOT virtual void setMaxImageAltTextChars(int value) noexcept override;
	Q_SIGNAL virtual void maxImageAltTextCharsChanged(int value) const override;

	virtual int maxImageSizeKb() const noexcept override;
	Q_SLOT virtual void setMaxImageSizeKb(int value) noexcept override;
	Q_SIGNAL virtual void maxImageSizeKbChanged(int value) const override;

	virtual int maxPageSizeKb() const noexcept override;
	Q_SLOT virtual void setMaxPageSizeKb(int value) noexcept override;
	Q_SIGNAL virtual void maxPageSizeKbChanged(int value) const override;

	virtual bool useProxy() const noexcept override;
	Q_SLOT virtual void setUseProxy(bool value) noexcept override;
	Q_SIGNAL virtual void useProxyChanged(int value) const override;

	virtual QString proxyHostName() const noexcept override;
	Q_SLOT virtual void setProxyHostName(const QString& value) override;
	Q_SIGNAL virtual void proxyHostNameChanged(const QString& value) const override;

	virtual int proxyPort() const noexcept override;
	Q_SLOT virtual void setProxyPort(int value) noexcept override;
	Q_SIGNAL virtual void proxyPortChanged(int value) const override;

	virtual QString proxyUser() const noexcept override;
	Q_SLOT virtual void setProxyUser(const QString& value) override;
	Q_SIGNAL virtual void proxyUserChanged(const QString& value) const override;

	virtual QString proxyPassword() const noexcept override;
	Q_SLOT virtual void setProxyPassword(const QString& value) override;
	Q_SIGNAL virtual void proxyPasswordChanged(const QString& value) const override;

	virtual bool checkExternalLinks() const noexcept override;
	Q_SLOT virtual void setCheckExternalLinks(bool value) noexcept override;
	Q_SIGNAL virtual void checkExternalLinksChanged(bool value) const override;

	virtual bool followInternalNofollow() const noexcept override;
	Q_SLOT virtual void setFollowInternalNofollow(bool value) noexcept override;
	Q_SIGNAL virtual void followInternalNofollowChanged(bool value) const override;

	virtual bool followExternalNofollow() const noexcept override;
	Q_SLOT virtual void setFollowExternalNofollow(bool value) noexcept override;
	Q_SIGNAL virtual void followExternalNofollowChanged(bool value) const override;

	virtual bool checkCanonicals() const noexcept override;
	Q_SLOT virtual void setCheckCanonicals(bool value) noexcept override;
	Q_SIGNAL virtual void checkCanonicalsChanged(bool value) const override;

	virtual bool checkSubdomains() const noexcept override;
	Q_SLOT virtual void setCheckSubdomains(bool value) noexcept override;
	Q_SIGNAL virtual void checkSubdomainsChanged(bool value) const override;

	virtual bool crawlOutsideOfStartFolder() const noexcept override;
	Q_SLOT virtual void setCrawlOutsideOfStartFolder(bool value) noexcept override;
	Q_SIGNAL virtual void crawlOutsideOfStartFolderChanged(bool value) const override;

	virtual bool crawlMetaHrefLangLinks() const noexcept override;
	Q_SLOT virtual void setCrawlMetaHrefLangLinks(bool value) noexcept override;
	Q_SIGNAL virtual void crawlMetaHrefLangLinksChanged(bool value) const override;

	virtual bool followRobotsTxtRules() const noexcept override;
	Q_SLOT virtual void setFollowRobotsTxtRules(bool value) noexcept override;
	Q_SIGNAL virtual void followRobotsTxtRulesChanged(bool value) const override;

	virtual UserAgentType userAgentToFollow() const noexcept override;
	Q_SLOT virtual void setUserAgentToFollow(UserAgentType value) noexcept override;
	Q_SIGNAL virtual void userAgentToFollowChanged(UserAgentType value) const override;

	virtual ParserTypeFlags parserTypeFlags() const noexcept override;
	Q_SLOT virtual void setParserTypeFlags(ParserTypeFlags value) noexcept override;
	Q_SIGNAL virtual void parserTypeFlagsChanged(ParserTypeFlags value) const override;

	virtual int pauseRangeFrom() const noexcept override;
	Q_SLOT virtual void setPauseRangeFrom(int value) noexcept override;
	Q_SIGNAL virtual void pauseRangeFromChanged(int value) const override;

	virtual int pauseRangeTo() const noexcept override;
	Q_SLOT virtual void setPauseRangeTo(int value) noexcept override;
	Q_SIGNAL virtual void pauseRangeToChanged(int value) const override;

	virtual bool pauseRangeEnabled() const noexcept override;
	Q_SLOT virtual void setPauseRangeEnabled(bool value) noexcept override;
	Q_SIGNAL virtual void pauseRangeEnabledChanged(bool value) const override;

	virtual QByteArray userAgent() const noexcept override;
	Q_SLOT virtual void setUserAgent(const QByteArray& value) override;
	Q_SIGNAL virtual void userAgentChanged(const QByteArray& value) const override;

	virtual QString excludeUrlRegExps() const noexcept override;
	Q_SLOT virtual void setExcludeUrlRegExps(const QString& value) override;
	Q_SIGNAL virtual void excludeUrlRegExpsChanged(const QString& value) const override;

	// yandex metrica

	virtual bool searchYandexMetricaCounters() const noexcept override;
	Q_SLOT virtual void setSearchYandexMetricaCounters(bool value) override;
	Q_SIGNAL virtual void searchYandexMetricaCountersChanged(bool value) override;

	virtual bool searchYandexMetricaCounter1() const noexcept override;
	Q_SLOT virtual void setSearchYandexMetricaCounter1(bool value) override;
	Q_SIGNAL virtual void searchYandexMetricaCounter1Changed(bool value) override;
	virtual int yandexMetricaCounter1Id() const noexcept override;
	Q_SLOT virtual void setYandexMetricaCounter1Id(int value) override;
	Q_SIGNAL virtual void yandexMetricaCounter1IdChanged(int value) override;

	virtual bool searchYandexMetricaCounter2() const noexcept override;
	Q_SLOT virtual void setSearchYandexMetricaCounter2(bool value) override;
	Q_SIGNAL virtual void searchYandexMetricaCounter2Changed(bool value) override;
	virtual int yandexMetricaCounter2Id() const noexcept override;
	Q_SLOT virtual void setYandexMetricaCounter2Id(int value) override;
	Q_SIGNAL virtual void yandexMetricaCounter2IdChanged(int value) override;

	virtual bool searchYandexMetricaCounter3() const noexcept override;
	Q_SLOT virtual void setSearchYandexMetricaCounter3(bool value) override;
	Q_SIGNAL virtual void searchYandexMetricaCounter3Changed(bool value) override;
	virtual int yandexMetricaCounter3Id() const noexcept override;
	Q_SLOT virtual void setYandexMetricaCounter3Id(int value) override;
	Q_SIGNAL virtual void yandexMetricaCounter3IdChanged(int value) override;

	virtual bool searchYandexMetricaCounter4() const noexcept override;
	Q_SLOT virtual void setSearchYandexMetricaCounter4(bool value) override;
	Q_SIGNAL virtual void searchYandexMetricaCounter4Changed(bool value) override;
	virtual int yandexMetricaCounter4Id() const noexcept override;
	Q_SLOT virtual void setYandexMetricaCounter4Id(int value) override;
	Q_SIGNAL virtual void yandexMetricaCounter4IdChanged(int value) override;

	virtual bool searchYandexMetricaCounter5() const noexcept override;
	Q_SLOT virtual void setSearchYandexMetricaCounter5(bool value) override;
	Q_SIGNAL virtual void searchYandexMetricaCounter5Changed(bool value) override;
	virtual int yandexMetricaCounter5Id() const noexcept override;
	Q_SLOT virtual void setYandexMetricaCounter5Id(int value) override;
	Q_SIGNAL virtual void yandexMetricaCounter5IdChanged(int value) override;

	// google analytics

	virtual bool searchGoogleAnalyticsCounters() const noexcept override;
	Q_SLOT virtual void setSearchGoogleAnalyticsCounters(bool value) override;
	Q_SIGNAL virtual void searchGoogleAnalyticsCountersChanged(bool value) override;

	virtual bool searchGoogleAnalyticsCounter1() const noexcept override;
	Q_SLOT virtual void setSearchGoogleAnalyticsCounter1(bool value) override;
	Q_SIGNAL virtual void searchGoogleAnalyticsCounter1Changed(bool value) override;
	virtual const QString& googleAnalyticsCounter1Id() const noexcept override;
	Q_SLOT virtual void setGoogleAnalyticsCounter1Id(const QString& value) override;
	Q_SIGNAL virtual void googleAnalyticsCounter1IdChanged(const QString& value) override;

	virtual bool searchGoogleAnalyticsCounter2() const noexcept override;
	Q_SLOT virtual void setSearchGoogleAnalyticsCounter2(bool value) override;
	Q_SIGNAL virtual void searchGoogleAnalyticsCounter2Changed(bool value) override;
	virtual const QString& googleAnalyticsCounter2Id() const noexcept override;
	Q_SLOT virtual void setGoogleAnalyticsCounter2Id(const QString& value) override;
	Q_SIGNAL virtual void googleAnalyticsCounter2IdChanged(const QString& value) override;

	virtual bool searchGoogleAnalyticsCounter3() const noexcept override;
	Q_SLOT virtual void setSearchGoogleAnalyticsCounter3(bool value) override;
	Q_SIGNAL virtual void searchGoogleAnalyticsCounter3Changed(bool value) override;
	virtual const QString& googleAnalyticsCounter3Id() const noexcept override;
	Q_SLOT virtual void setGoogleAnalyticsCounter3Id(const QString& value) override;
	Q_SIGNAL virtual void googleAnalyticsCounter3IdChanged(const QString& value) override;

	virtual bool searchGoogleAnalyticsCounter4() const noexcept override;
	Q_SLOT virtual void setSearchGoogleAnalyticsCounter4(bool value) override;
	Q_SIGNAL virtual void searchGoogleAnalyticsCounter4Changed(bool value) override;
	virtual const QString& googleAnalyticsCounter4Id() const noexcept override;
	Q_SLOT virtual void setGoogleAnalyticsCounter4Id(const QString& value) override;
	Q_SIGNAL virtual void googleAnalyticsCounter4IdChanged(const QString& value) override;

	virtual bool searchGoogleAnalyticsCounter5() const noexcept override;
	Q_SLOT virtual void setSearchGoogleAnalyticsCounter5(bool value) override;
	Q_SIGNAL virtual void searchGoogleAnalyticsCounter5Changed(bool value) override;
	virtual const QString& googleAnalyticsCounter5Id() const noexcept override;
	Q_SLOT virtual void setGoogleAnalyticsCounter5Id(const QString& value) override;
	Q_SIGNAL virtual void googleAnalyticsCounter5IdChanged(const QString& value) override;

    // Data Extraction
    virtual const QString& extractorName1() const noexcept override;
    virtual const QString& extractorName2() const noexcept override;
    virtual const QString& extractorName3() const noexcept override;
    virtual const QString& extractorName4() const noexcept override;
    virtual const QString& extractorName5() const noexcept override;
    virtual const QString& extractorName6() const noexcept override;
    virtual const QString& extractorName7() const noexcept override;
    virtual const QString& extractorName8() const noexcept override;
    virtual const QString& extractorName9() const noexcept override;
    virtual const QString& extractorName10() const noexcept override;
    Q_SLOT virtual void setExtractorName1(const QString& value) override;
    Q_SLOT virtual void setExtractorName2(const QString& value) override;
    Q_SLOT virtual void setExtractorName3(const QString& value) override;
    Q_SLOT virtual void setExtractorName4(const QString& value) override;
    Q_SLOT virtual void setExtractorName5(const QString& value) override;
    Q_SLOT virtual void setExtractorName6(const QString& value) override;
    Q_SLOT virtual void setExtractorName7(const QString& value) override;
    Q_SLOT virtual void setExtractorName8(const QString& value) override;
    Q_SLOT virtual void setExtractorName9(const QString& value) override;
    Q_SLOT virtual void setExtractorName10(const QString& value) override;
    Q_SIGNAL virtual void extractorName1Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName2Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName3Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName4Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName5Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName6Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName7Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName8Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName9Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorName10Changed(const QString& value) override;

    virtual int extractorRuleType1() const noexcept override;
    virtual int extractorRuleType2() const noexcept override;
    virtual int extractorRuleType3() const noexcept override;
    virtual int extractorRuleType4() const noexcept override;
    virtual int extractorRuleType5() const noexcept override;
    virtual int extractorRuleType6() const noexcept override;
    virtual int extractorRuleType7() const noexcept override;
    virtual int extractorRuleType8() const noexcept override;
    virtual int extractorRuleType9() const noexcept override;
    virtual int extractorRuleType10() const noexcept override;
    Q_SLOT virtual void setExtractorRuleType1(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType2(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType3(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType4(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType5(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType6(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType7(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType8(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType9(int value) noexcept override;
    Q_SLOT virtual void setExtractorRuleType10(int value) noexcept override;
    Q_SIGNAL virtual void extractorRuleType1Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType2Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType3Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType4Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType5Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType6Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType7Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType8Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType9Changed(int value) override;
    Q_SIGNAL virtual void extractorRuleType10Changed(int value) override;

    virtual const QString& extractorRule1() const noexcept override;
    virtual const QString& extractorRule2() const noexcept override;
    virtual const QString& extractorRule3() const noexcept override;
    virtual const QString& extractorRule4() const noexcept override;
    virtual const QString& extractorRule5() const noexcept override;
    virtual const QString& extractorRule6() const noexcept override;
    virtual const QString& extractorRule7() const noexcept override;
    virtual const QString& extractorRule8() const noexcept override;
    virtual const QString& extractorRule9() const noexcept override;
    virtual const QString& extractorRule10() const noexcept override;
    Q_SLOT virtual void setExtractorRule1(const QString& value) override;
    Q_SLOT virtual void setExtractorRule2(const QString& value) override;
    Q_SLOT virtual void setExtractorRule3(const QString& value) override;
    Q_SLOT virtual void setExtractorRule4(const QString& value) override;
    Q_SLOT virtual void setExtractorRule5(const QString& value) override;
    Q_SLOT virtual void setExtractorRule6(const QString& value) override;
    Q_SLOT virtual void setExtractorRule7(const QString& value) override;
    Q_SLOT virtual void setExtractorRule8(const QString& value) override;
    Q_SLOT virtual void setExtractorRule9(const QString& value) override;
    Q_SLOT virtual void setExtractorRule10(const QString& value) override;
    Q_SIGNAL virtual void extractorRule1Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule2Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule3Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule4Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule5Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule6Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule7Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule8Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule9Changed(const QString& value) override;
    Q_SIGNAL virtual void extractorRule10Changed(const QString& value) override;

    virtual int extractionType1() const noexcept override;
    virtual int extractionType2() const noexcept override;
    virtual int extractionType3() const noexcept override;
    virtual int extractionType4() const noexcept override;
    virtual int extractionType5() const noexcept override;
    virtual int extractionType6() const noexcept override;
    virtual int extractionType7() const noexcept override;
    virtual int extractionType8() const noexcept override;
    virtual int extractionType9() const noexcept override;
    virtual int extractionType10() const noexcept override;
    Q_SLOT virtual void setExtractionType1(int value) noexcept override;
    Q_SLOT virtual void setExtractionType2(int value) noexcept override;
    Q_SLOT virtual void setExtractionType3(int value) noexcept override;
    Q_SLOT virtual void setExtractionType4(int value) noexcept override;
    Q_SLOT virtual void setExtractionType5(int value) noexcept override;
    Q_SLOT virtual void setExtractionType6(int value) noexcept override;
    Q_SLOT virtual void setExtractionType7(int value) noexcept override;
    Q_SLOT virtual void setExtractionType8(int value) noexcept override;
    Q_SLOT virtual void setExtractionType9(int value) noexcept override;
    Q_SLOT virtual void setExtractionType10(int value) noexcept override;
    Q_SIGNAL virtual void extractionType1Changed(int value) override;
    Q_SIGNAL virtual void extractionType2Changed(int value) override;
    Q_SIGNAL virtual void extractionType3Changed(int value) override;
    Q_SIGNAL virtual void extractionType4Changed(int value) override;
    Q_SIGNAL virtual void extractionType5Changed(int value) override;
    Q_SIGNAL virtual void extractionType6Changed(int value) override;
    Q_SIGNAL virtual void extractionType7Changed(int value) override;
    Q_SIGNAL virtual void extractionType8Changed(int value) override;
    Q_SIGNAL virtual void extractionType9Changed(int value) override;
    Q_SIGNAL virtual void extractionType10Changed(int value) override;

private:
	CrawlerOptionsData m_data;
};

Q_DECLARE_METATYPE(CrawlerOptionsData)

}