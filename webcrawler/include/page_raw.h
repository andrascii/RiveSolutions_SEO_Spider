#pragma once

namespace WebCrawler
{

struct PageRaw
{
	QUrl url;
	QUrl fromUrl;
	QString title;
	QString content;
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

};

using PageRawPtr = std::shared_ptr<PageRaw>;

Q_DECLARE_METATYPE(PageRawPtr);

}