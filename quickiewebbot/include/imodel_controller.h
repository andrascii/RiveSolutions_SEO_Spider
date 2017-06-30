#pragma once

namespace QuickieWebBot
{

struct PayloadElement
{	
	int statusCode;
		
	QString title;
	int titleLength;

	QString metaDescription;
	int metaDescriptionLength;

	QString metaKeywords;
	int metaKeywordsLength;

	QString firstH1;
	int firstH1Length;

	QString secondH1;
	int secondH1Length;

	QString firstH2;
	int firstH2Length;

	QString secondH2;
	int secondH2Length;

	QString canonicalLinkElement;
	int pageSizeBytes;
	int wordCount;
	size_t hash;

	QUrl url;
	QString content;
	QString metaRefresh;
	QString metaRobots;
	QString redirectedUrl;
	QString serverResponse;
};

class IModelController
{
public:
	virtual void setUrl(QUrl const& url) = 0;
	virtual QUrl const& url() const = 0;


};

}