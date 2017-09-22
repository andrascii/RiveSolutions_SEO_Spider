#pragma once

namespace WebCrawler
{

struct PageRaw;

using PageRawWeakPtr = std::weak_ptr<PageRaw>;


enum class UrlParameter
{
	UnknownParameter,
	NofollowParameter,
	DofollowParameter
};

enum class ResourceType
{
	ResourceHtml,
	ResourceImage,
	ResourceJavaScript,
	ResourceStyleSheet,
	ResourceFlash,
	ResourceVideo,
	ResourceOther
};

struct OnPageUrl
{
	QUrl url;
	UrlParameter urlParameter;
};

struct ResourceLink
{
	PageRawWeakPtr resource;
	UrlParameter urlParameter;
};

struct RawResourceOnPage
{
	ResourceType resourceType;
	OnPageUrl thisResourceUrl;
};

struct PageRaw
{
	QUrl url;
	QUrl redirectedUrl;

	QString title;
	QString contentType;
	QString metaRefresh;
	QString metaRobots;
	QString metaDescription;
	QString metaKeywords;
	QString serverResponse;
	QString firstH1;
	QString secondH1;
	QString firstH2;
	QString secondH2;
	QString canonicalLinkElement;
	QString altText;
	int statusCode;
	int pageSizeKb;
	int wordCount;
	size_t pageHash;

	UrlParameter linkParameter;

	bool hasSeveralTitleTags;
	bool hasSeveralMetaDescriptionTags;
	bool hasSeveralMetaKeywordsTags;
	bool hasSeveralH1Tags;
	bool hasSeveralH2Tags;

	bool isThisExternalPage;

	ResourceType resourceType;

	std::deque<RawResourceOnPage> allResourcesOnPage;
	std::deque<ResourceLink> linksOnThisPage;
	std::deque<ResourceLink> linksToThisPage;

#ifdef QT_DEBUG
	QByteArray rawHtml;
#endif
};

using PageRawPtr = std::shared_ptr<PageRaw>;

Q_DECLARE_METATYPE(PageRawPtr);
Q_DECLARE_METATYPE(PageRawWeakPtr);

}