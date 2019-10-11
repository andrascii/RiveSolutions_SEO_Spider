#pragma once

#include "robots_txt_rules.h"

namespace CrawlerEngine
{

enum class UserAgentType;

enum ParserType
{
	JavaScriptResourcesParserType = 1 << 0,
	CssResourcesParserType = 1 << 1,
	ImagesResourcesParserType = 1 << 2,
	VideoResourcesParserType = 1 << 3,
	FlashResourcesParserType = 1 << 4,
	OtherResourcesParserType = 1 << 5
};

Q_DECLARE_FLAGS(ParserTypeFlags, ParserType)

struct CrawlerOptionsData;

class ICrawlerOptions
{
public:
	virtual QObject* qobject() const noexcept = 0;

	virtual const CrawlerOptionsData& data() const noexcept = 0;
	virtual void setData(const CrawlerOptionsData& data) noexcept = 0;
	virtual void setData(CrawlerOptionsData&& data) noexcept = 0;
	virtual void dataChanged(const CrawlerOptionsData& data) const = 0;

	virtual Url startCrawlingPage() const noexcept = 0;
	virtual void setStartCrawlingPage(const Url& url) = 0;
	virtual void startCrawlingPageChanged(const Url& url) const = 0;

	virtual int limitMaxUrlLength() const noexcept = 0;
	virtual void setLimitMaxUrlLength(int value) noexcept = 0;
	virtual void limitMaxUrlLengthChanged(int value) const = 0;

	virtual int limitSearchTotal() const noexcept = 0;
	virtual void setLimitSearchTotal(int value) noexcept = 0;
	virtual void limitSearchTotalChanged(int value) const = 0;

	virtual int limitTimeout() const noexcept = 0;
	virtual void setLimitTimeout(int value) noexcept = 0;
	virtual int limitTimeoutChanged(int value) const = 0;

	virtual int maxRedirectsToFollow() const noexcept = 0;
	virtual void setMaxRedirectsToFollow(int value) noexcept = 0;
	virtual void maxRedirectsToFollowChanged(int value) const = 0;

	virtual int maxParallelConnections() const noexcept = 0;
	virtual void setMaxParallelConnections(int value) noexcept = 0;
	virtual void maxParallelConnectionsChanged(int value) const = 0;

	virtual int maxLinksCountOnPage() const noexcept = 0;
	virtual void setMaxLinksCountOnPage(int value) noexcept = 0;
	virtual void maxLinksCountOnPageChanged(int value) const = 0;

	virtual int maxTitleLength() const noexcept = 0;
	virtual void setMaxTitleLength(int value) noexcept = 0;
	virtual void maxTitleLengthChanged(int value) const = 0;

	virtual int minTitleLength() const noexcept = 0;
	virtual void setMinTitleLength(int value) noexcept = 0;
	virtual void minTitleLengthChanged(int value) const = 0;

	virtual int maxDescriptionLength() const noexcept = 0;
	virtual void setMaxDescriptionLength(int value) noexcept = 0;
	virtual void maxDescriptionLengthChanged(int value) const = 0;

	virtual int minDescriptionLength() const noexcept = 0;
	virtual void setMinDescriptionLength(int value) noexcept = 0;
	virtual void minDescriptionLengthChanged(int value) const = 0;

	virtual int maxH1LengthChars() const noexcept = 0;
	virtual void setMaxH1LengthChars(int value) noexcept = 0;
	virtual void maxH1LengthCharsChanged(int value) const = 0;

	virtual int maxH2LengthChars() const noexcept = 0;
	virtual void setMaxH2LengthChars(int value) noexcept = 0;
	virtual void maxH2LengthCharsChanged(int value) const = 0;

	virtual int maxImageAltTextChars() const noexcept = 0;
	virtual void setMaxImageAltTextChars(int value) noexcept = 0;
	virtual void maxImageAltTextCharsChanged(int value) const = 0;

	virtual int maxImageSizeKb() const noexcept = 0;
	virtual void setMaxImageSizeKb(int value) noexcept = 0;
	virtual void maxImageSizeKbChanged(int value) const = 0;

	virtual int maxPageSizeKb() const noexcept = 0;
	virtual void setMaxPageSizeKb(int value) noexcept = 0;
	virtual void maxPageSizeKbChanged(int value) const = 0;

	virtual bool useProxy() const noexcept = 0;
	virtual void setUseProxy(bool value) noexcept = 0;
	virtual void useProxyChanged(int value) const = 0;

	virtual QString proxyHostName() const noexcept = 0;
	virtual void setProxyHostName(const QString& value) = 0;
	virtual void proxyHostNameChanged(const QString& value) const = 0;

	virtual int proxyPort() const noexcept = 0;
	virtual void setProxyPort(int value) noexcept = 0;
	virtual void proxyPortChanged(int value) const = 0;

	virtual QString proxyUser() const noexcept = 0;
	virtual void setProxyUser(const QString& value) = 0;
	virtual void proxyUserChanged(const QString& value) const = 0;

	virtual QString proxyPassword() const noexcept = 0;
	virtual void setProxyPassword(const QString& value) = 0;
	virtual void proxyPasswordChanged(const QString& value) const = 0;

	virtual bool checkExternalLinks() const noexcept = 0;
	virtual void setCheckExternalLinks(bool value) noexcept = 0;
	virtual void checkExternalLinksChanged(bool value) const = 0;

	virtual bool followInternalNofollow() const noexcept = 0;
	virtual void setFollowInternalNofollow(bool value) noexcept = 0;
	virtual void followInternalNofollowChanged(bool value) const = 0;

	virtual bool followExternalNofollow() const noexcept = 0;
	virtual void setFollowExternalNofollow(bool value) noexcept = 0;
	virtual void followExternalNofollowChanged(bool value) const = 0;

	virtual bool checkCanonicals() const noexcept = 0;
	virtual void setCheckCanonicals(bool value) noexcept = 0;
	virtual void checkCanonicalsChanged(bool value) const = 0;

	virtual bool checkSubdomains() const noexcept = 0;
	virtual void setCheckSubdomains(bool value) noexcept = 0;
	virtual void checkSubdomainsChanged(bool value) const = 0;

	virtual bool crawlOutsideOfStartFolder() const noexcept = 0;
	virtual void setCrawlOutsideOfStartFolder(bool value) noexcept = 0;
	virtual void crawlOutsideOfStartFolderChanged(bool value) const = 0;

	virtual bool crawlMetaHrefLangLinks() const noexcept = 0;
	virtual void setCrawlMetaHrefLangLinks(bool value) noexcept = 0;
	virtual void crawlMetaHrefLangLinksChanged(bool value) const = 0;

	virtual bool followRobotsTxtRules() const noexcept = 0;
	virtual void setFollowRobotsTxtRules(bool value) noexcept = 0;
	virtual void followRobotsTxtRulesChanged(bool value) const = 0;

	virtual UserAgentType userAgentToFollow() const noexcept = 0;
	virtual void setUserAgentToFollow(UserAgentType value) noexcept = 0;
	virtual void userAgentToFollowChanged(UserAgentType value) const = 0;

	virtual ParserTypeFlags parserTypeFlags() const noexcept = 0;
	virtual void setParserTypeFlags(ParserTypeFlags value) noexcept = 0;
	virtual void parserTypeFlagsChanged(ParserTypeFlags value) const = 0;

	virtual int pauseRangeFrom() const noexcept = 0;
	virtual void setPauseRangeFrom(int value) noexcept = 0;
	virtual void pauseRangeFromChanged(int value) const = 0;

	virtual int pauseRangeTo() const noexcept = 0;
	virtual void setPauseRangeTo(int value) noexcept = 0;
	virtual void pauseRangeToChanged(int value) const = 0;

	virtual bool pauseRangeEnabled() const noexcept = 0;
	virtual void setPauseRangeEnabled(bool value) noexcept = 0;
	virtual void pauseRangeEnabledChanged(bool value) const = 0;

	virtual QByteArray userAgent() const noexcept = 0;
	virtual void setUserAgent(const QByteArray& value) = 0;
	virtual void userAgentChanged(const QByteArray& value) const = 0;

	virtual QString excludeUrlRegExps() const noexcept = 0;
	virtual void setExcludeUrlRegExps(const QString& value) = 0;
	virtual void excludeUrlRegExpsChanged(const QString& value) const = 0;

	// ym
	virtual bool searchYandexMetricaCounters() const noexcept = 0;
	virtual void setSearchYandexMetricaCounters(bool value) = 0;
	virtual void searchYandexMetricaCountersChanged(bool value) = 0;

	virtual bool searchYandexMetricaCounter1() const noexcept = 0;
	virtual void setSearchYandexMetricaCounter1(bool value) = 0;
	virtual void searchYandexMetricaCounter1Changed(bool value) = 0;
	virtual int yandexMetricaCounter1Id() const noexcept = 0;
	virtual void setYandexMetricaCounter1Id(int value) = 0;
	virtual void yandexMetricaCounter1IdChanged(int value) = 0;

	virtual bool searchYandexMetricaCounter2() const noexcept = 0;
	virtual void setSearchYandexMetricaCounter2(bool value) = 0;
	virtual void searchYandexMetricaCounter2Changed(bool value) = 0;
	virtual int yandexMetricaCounter2Id() const noexcept = 0;
	virtual void setYandexMetricaCounter2Id(int value) = 0;
	virtual void yandexMetricaCounter2IdChanged(int value) = 0;

	virtual bool searchYandexMetricaCounter3() const noexcept = 0;
	virtual void setSearchYandexMetricaCounter3(bool value) = 0;
	virtual void searchYandexMetricaCounter3Changed(bool value) = 0;
	virtual int yandexMetricaCounter3Id() const noexcept = 0;
	virtual void setYandexMetricaCounter3Id(int value) = 0;
	virtual void yandexMetricaCounter3IdChanged(int value) = 0;

	virtual bool searchYandexMetricaCounter4() const noexcept = 0;
	virtual void setSearchYandexMetricaCounter4(bool value) = 0;
	virtual void searchYandexMetricaCounter4Changed(bool value) = 0;
	virtual int yandexMetricaCounter4Id() const noexcept = 0;
	virtual void setYandexMetricaCounter4Id(int value) = 0;
	virtual void yandexMetricaCounter4IdChanged(int value) = 0;

	virtual bool searchYandexMetricaCounter5() const noexcept = 0;
	virtual void setSearchYandexMetricaCounter5(bool value) = 0;
	virtual void searchYandexMetricaCounter5Changed(bool value) = 0;
	virtual int yandexMetricaCounter5Id() const noexcept = 0;
	virtual void setYandexMetricaCounter5Id(int value) = 0;
	virtual void yandexMetricaCounter5IdChanged(int value) = 0;

	// ga
	virtual bool searchGoogleAnalyticsCounters() const noexcept = 0;
	virtual void setSearchGoogleAnalyticsCounters(bool value) = 0;
	virtual void searchGoogleAnalyticsCountersChanged(bool value) = 0;

	virtual bool searchGoogleAnalyticsCounter1() const noexcept = 0;
	virtual void setSearchGoogleAnalyticsCounter1(bool value) = 0;
	virtual void searchGoogleAnalyticsCounter1Changed(bool value) = 0;
	virtual const QString& googleAnalyticsCounter1Id() const noexcept = 0;
	virtual void setGoogleAnalyticsCounter1Id(const QString& value) = 0;
	virtual void googleAnalyticsCounter1IdChanged(const QString& value) = 0;

	virtual bool searchGoogleAnalyticsCounter2() const noexcept = 0;
	virtual void setSearchGoogleAnalyticsCounter2(bool value) = 0;
	virtual void searchGoogleAnalyticsCounter2Changed(bool value) = 0;
	virtual const QString& googleAnalyticsCounter2Id() const noexcept = 0;
	virtual void setGoogleAnalyticsCounter2Id(const QString& value) = 0;
	virtual void googleAnalyticsCounter2IdChanged(const QString& value) = 0;

	virtual bool searchGoogleAnalyticsCounter3() const noexcept = 0;
	virtual void setSearchGoogleAnalyticsCounter3(bool value) = 0;
	virtual void searchGoogleAnalyticsCounter3Changed(bool value) = 0;
	virtual const QString& googleAnalyticsCounter3Id() const noexcept = 0;
	virtual void setGoogleAnalyticsCounter3Id(const QString& value) = 0;
	virtual void googleAnalyticsCounter3IdChanged(const QString& value) = 0;

	virtual bool searchGoogleAnalyticsCounter4() const noexcept = 0;
	virtual void setSearchGoogleAnalyticsCounter4(bool value) = 0;
	virtual void searchGoogleAnalyticsCounter4Changed(bool value) = 0;
	virtual const QString& googleAnalyticsCounter4Id() const noexcept = 0;
	virtual void setGoogleAnalyticsCounter4Id(const QString& value) = 0;
	virtual void googleAnalyticsCounter4IdChanged(const QString& value) = 0;

	virtual bool searchGoogleAnalyticsCounter5() const noexcept = 0;
	virtual void setSearchGoogleAnalyticsCounter5(bool value) = 0;
	virtual void searchGoogleAnalyticsCounter5Changed(bool value) = 0;
	virtual const QString& googleAnalyticsCounter5Id() const noexcept = 0;
	virtual void setGoogleAnalyticsCounter5Id(const QString& value) = 0;
	virtual void googleAnalyticsCounter5IdChanged(const QString& value) = 0;

    // data extraction
    virtual const QString& extractorName1() const noexcept = 0;
    virtual const QString& extractorName2() const noexcept = 0;
    virtual const QString& extractorName3() const noexcept = 0;
    virtual const QString& extractorName4() const noexcept = 0;
    virtual const QString& extractorName5() const noexcept = 0;
    virtual const QString& extractorName6() const noexcept = 0;
    virtual const QString& extractorName7() const noexcept = 0;
    virtual const QString& extractorName8() const noexcept = 0;
    virtual const QString& extractorName9() const noexcept = 0;
    virtual const QString& extractorName10() const noexcept = 0;
    virtual void setExtractorName1(const QString& value) = 0;
    virtual void setExtractorName2(const QString& value) = 0;
    virtual void setExtractorName3(const QString& value) = 0;
    virtual void setExtractorName4(const QString& value) = 0;
    virtual void setExtractorName5(const QString& value) = 0;
    virtual void setExtractorName6(const QString& value) = 0;
    virtual void setExtractorName7(const QString& value) = 0;
    virtual void setExtractorName8(const QString& value) = 0;
    virtual void setExtractorName9(const QString& value) = 0;
    virtual void setExtractorName10(const QString& value) = 0;
    virtual void extractorName1Changed(const QString& value) = 0;
    virtual void extractorName2Changed(const QString& value) = 0;
    virtual void extractorName3Changed(const QString& value) = 0;
    virtual void extractorName4Changed(const QString& value) = 0;
    virtual void extractorName5Changed(const QString& value) = 0;
    virtual void extractorName6Changed(const QString& value) = 0;
    virtual void extractorName7Changed(const QString& value) = 0;
    virtual void extractorName8Changed(const QString& value) = 0;
    virtual void extractorName9Changed(const QString& value) = 0;
    virtual void extractorName10Changed(const QString& value) = 0;

    virtual int extractorRuleType1() const noexcept = 0;
    virtual int extractorRuleType2() const noexcept = 0;
    virtual int extractorRuleType3() const noexcept = 0;
    virtual int extractorRuleType4() const noexcept = 0;
    virtual int extractorRuleType5() const noexcept = 0;
    virtual int extractorRuleType6() const noexcept = 0;
    virtual int extractorRuleType7() const noexcept = 0;
    virtual int extractorRuleType8() const noexcept = 0;
    virtual int extractorRuleType9() const noexcept = 0;
    virtual int extractorRuleType10() const noexcept = 0;
    virtual void setExtractorRuleType1(int value) noexcept = 0;
    virtual void setExtractorRuleType2(int value) noexcept = 0;
    virtual void setExtractorRuleType3(int value) noexcept = 0;
    virtual void setExtractorRuleType4(int value) noexcept = 0;
    virtual void setExtractorRuleType5(int value) noexcept = 0;
    virtual void setExtractorRuleType6(int value) noexcept = 0;
    virtual void setExtractorRuleType7(int value) noexcept = 0;
    virtual void setExtractorRuleType8(int value) noexcept = 0;
    virtual void setExtractorRuleType9(int value) noexcept = 0;
    virtual void setExtractorRuleType10(int value) noexcept = 0;
    virtual void extractorRuleType1Changed(int value) = 0;
    virtual void extractorRuleType2Changed(int value) = 0;
    virtual void extractorRuleType3Changed(int value) = 0;
    virtual void extractorRuleType4Changed(int value) = 0;
    virtual void extractorRuleType5Changed(int value) = 0;
    virtual void extractorRuleType6Changed(int value) = 0;
    virtual void extractorRuleType7Changed(int value) = 0;
    virtual void extractorRuleType8Changed(int value) = 0;
    virtual void extractorRuleType9Changed(int value) = 0;
    virtual void extractorRuleType10Changed(int value) = 0;

    virtual const QString& extractorRule1() const noexcept = 0;
    virtual const QString& extractorRule2() const noexcept = 0;
    virtual const QString& extractorRule3() const noexcept = 0;
    virtual const QString& extractorRule4() const noexcept = 0;
    virtual const QString& extractorRule5() const noexcept = 0;
    virtual const QString& extractorRule6() const noexcept = 0;
    virtual const QString& extractorRule7() const noexcept = 0;
    virtual const QString& extractorRule8() const noexcept = 0;
    virtual const QString& extractorRule9() const noexcept = 0;
    virtual const QString& extractorRule10() const noexcept = 0;
    virtual void setExtractorRule1(const QString& value) = 0;
    virtual void setExtractorRule2(const QString& value) = 0;
    virtual void setExtractorRule3(const QString& value) = 0;
    virtual void setExtractorRule4(const QString& value) = 0;
    virtual void setExtractorRule5(const QString& value) = 0;
    virtual void setExtractorRule6(const QString& value) = 0;
    virtual void setExtractorRule7(const QString& value) = 0;
    virtual void setExtractorRule8(const QString& value) = 0;
    virtual void setExtractorRule9(const QString& value) = 0;
    virtual void setExtractorRule10(const QString& value) = 0;
    virtual void extractorRule1Changed(const QString& value) = 0;
    virtual void extractorRule2Changed(const QString& value) = 0;
    virtual void extractorRule3Changed(const QString& value) = 0;
    virtual void extractorRule4Changed(const QString& value) = 0;
    virtual void extractorRule5Changed(const QString& value) = 0;
    virtual void extractorRule6Changed(const QString& value) = 0;
    virtual void extractorRule7Changed(const QString& value) = 0;
    virtual void extractorRule8Changed(const QString& value) = 0;
    virtual void extractorRule9Changed(const QString& value) = 0;
    virtual void extractorRule10Changed(const QString& value) = 0;

    virtual int extractionType1() const noexcept = 0;
    virtual int extractionType2() const noexcept = 0;
    virtual int extractionType3() const noexcept = 0;
    virtual int extractionType4() const noexcept = 0;
    virtual int extractionType5() const noexcept = 0;
    virtual int extractionType6() const noexcept = 0;
    virtual int extractionType7() const noexcept = 0;
    virtual int extractionType8() const noexcept = 0;
    virtual int extractionType9() const noexcept = 0;
    virtual int extractionType10() const noexcept = 0;
    virtual void setExtractionType1(int value) noexcept = 0;
    virtual void setExtractionType2(int value) noexcept = 0;
    virtual void setExtractionType3(int value) noexcept = 0;
    virtual void setExtractionType4(int value) noexcept = 0;
    virtual void setExtractionType5(int value) noexcept = 0;
    virtual void setExtractionType6(int value) noexcept = 0;
    virtual void setExtractionType7(int value) noexcept = 0;
    virtual void setExtractionType8(int value) noexcept = 0;
    virtual void setExtractionType9(int value) noexcept = 0;
    virtual void setExtractionType10(int value) noexcept = 0;
    virtual void extractionType1Changed(int value) = 0;
    virtual void extractionType2Changed(int value) = 0;
    virtual void extractionType3Changed(int value) = 0;
    virtual void extractionType4Changed(int value) = 0;
    virtual void extractionType5Changed(int value) = 0;
    virtual void extractionType6Changed(int value) = 0;
    virtual void extractionType7Changed(int value) = 0;
    virtual void extractionType8Changed(int value) = 0;
    virtual void extractionType9Changed(int value) = 0;
    virtual void extractionType10Changed(int value) = 0;
};

}