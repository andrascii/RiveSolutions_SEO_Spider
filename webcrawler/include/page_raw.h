#pragma once

namespace WebCrawler
{

struct PageRaw;
using PageRawWeakPtr = std::weak_ptr<PageRaw>;

struct Link
{
	enum LinkParameter
	{
		UnknownParameter,
		NofollowParameter,
		DofollowParameter
	};

	QUrl url;
	LinkParameter linkParameter;

};

struct ResourceLink
{
	PageRawWeakPtr resource;
	Link::LinkParameter linkParameter;
};

struct PageRawResource
{
	enum ResourceType
	{
		ResourceHtml,
		ResourceImage,
		ResourceJavaScript,
		ResourceStyleSheet,
		ResourceFlash,
		ResourceVideo,
		ResourceOther
	};

	ResourceType resourceType;
	Link resourceLink;
	QUrl containingThisResourceUrl;
};

struct PageRaw
{
	QUrl url;
	QUrl fromUrl;
	QString title;
	QString contentType;
	QString metaRefresh;
	QString metaRobots;
	QString metaDescription;
	QString metaKeywords;
	QUrl redirectedUrl;
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

	Link::LinkParameter linkParameter;

	bool hasSeveralTitleTags;
	bool hasSeveralMetaDescriptionTags;
	bool hasSeveralMetaKeywordsTags;
	bool hasSeveralH1Tags;
	bool hasSeveralH2Tags;

	bool isExternal;

	PageRawResource::ResourceType resourceType;

	std::deque<PageRawResource> rawResources; // TODO: move to another structure
	
	std::deque<ResourceLink> linksFromThisPage;
	std::deque<ResourceLink> linksToThisPage;

#ifdef QT_DEBUG

	QByteArray rawHtml;

#endif
};

using PageRawPtr = std::shared_ptr<PageRaw>;

Q_DECLARE_METATYPE(PageRawPtr);
Q_DECLARE_METATYPE(PageRawWeakPtr);

}