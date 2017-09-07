#pragma once

namespace WebCrawler
{

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
	QUrl resourceUrl;
	QUrl resourcePageUrl;
};


struct PageRaw
{
	using PageRawWeakPtr = std::weak_ptr<PageRaw>;

	QUrl url;
	QUrl fromUrl;
	QString title;
	QString contentType;
	QString metaRefresh;
	QString metaRobots;
	QString metaDescription;
	QString metaKeywords;
	QString redirectedUrl;
	QString serverResponse;
	QString firstH1;
	QString secondH1;
	QString firstH2;
	QString secondH2;
	QString canonicalLinkElement;
	QString imageAltText;
	int statusCode;
	int pageSizeKb;
	int wordCount;
	size_t pageHash;

	bool hasSeveralTitleTags;
	bool hasSeveralMetaDescriptionTags;
	bool hasSeveralMetaKeywordsTags;
	bool hasSeveralH1Tags;
	bool hasSeveralH2Tags;

	PageRawResource::ResourceType resourceType;

	std::deque<PageRawResource> rawResources; // TODO: move to another structure
	
	std::deque<PageRawWeakPtr> linksFromThisPage;
	std::deque<PageRawWeakPtr> linksToThisPage;

#ifdef QT_DEBUG

	QByteArray rawHtml;

#endif
};

using PageRawPtr = std::shared_ptr<PageRaw>;

Q_DECLARE_METATYPE(PageRawPtr);

}