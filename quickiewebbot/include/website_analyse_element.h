#pragma once

namespace QuickieWebBot
{

struct WebsiteAnalyseElement
{
	QUrl url;

	QString content;

	QString metaRefresh;
	QString metaRobots;

	QString redirectedUrl;
	QString serverResponse;

	QString title;
	QString metaDescription;
	QString metaKeywords;

	QString firstH1;
	QString secondH1;

	QString firstH2;
	QString secondH2;

	QString canonicalLinkElement;

	int statusCode;
	int titleLength;

	int metaDescriptionLength;
	int metaKeywordsLength;

	int firstH1Length;
	int secondH1Length;

	int firstH2Length;
	int secondH2Length;

	int pageSizeBytes;
	int wordCount;

	size_t pageHash;
};

struct WebsiteAnalyseElementHasher
{
	size_t operator()(WebsiteAnalyseElement const* websiteAnalyseElement) const noexcept
	{
		return hasher(websiteAnalyseElement->url.toString().toStdString());
	}

	boost::hash<std::string> hasher;
};

}
