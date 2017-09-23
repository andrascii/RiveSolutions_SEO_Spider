#pragma once

namespace WebCrawler
{

struct ParsedPage;

using ParsedPageWeakPtr = std::weak_ptr<ParsedPage>;


enum class LinkParameter
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

struct LinkInfo
{
	QUrl url;
	LinkParameter urlParameter;
};

struct ResourceLink
{
	ParsedPageWeakPtr resource;
	LinkParameter urlParameter;
};

struct RawResourceOnPage
{
	ResourceType resourceType;
	LinkInfo thisResourceUrl;
};

struct ParsedPage
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

	LinkParameter linkParameter;

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

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr);
Q_DECLARE_METATYPE(ParsedPageWeakPtr);

}