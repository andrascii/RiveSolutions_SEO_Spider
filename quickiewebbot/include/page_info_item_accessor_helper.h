#pragma once

#include "page_info.h"

namespace QuickieWebBot
{

class PageInfoItemAccessorHelper
{
public:
	enum ItemInfo 
	{ 
		Url,
		Title,
		MetaRefresh,
		MetaRobots,
		RedirectedUrl,
		ServerResponse,
		MetaDescription,
		MetaKeywords,
		FirstH1,
		SecondH1,
		FirstH2,
		SecondH2,
		CanonicalLinkElement,
		StatusCode,
		TitleLength,
		MetaDescriptionLength,
		MetaKeywordsLength,
		FirstH1Length,
		SecondH1Length,
		FirstH2Length,
		SecondH2Length,
		PageSizeBytes,
		WordCount
	};

	static QString title(ItemInfo item);
	static QVariant value(PageInfoPtr pageInfo, ItemInfo item);

private: 
	using GetterInternal = QVariant(*)(PageInfoPtr);

	static GetterInternal acceptItem(ItemInfo item);
	static QVariant url(PageInfoPtr pageInfo);
	static QVariant title(PageInfoPtr pageInfo);
	static QVariant metaRefresh(PageInfoPtr pageInfo);
	static QVariant metaRobots(PageInfoPtr pageInfo);
	static QVariant redirectedUrl(PageInfoPtr pageInfo);
	static QVariant serverResponse(PageInfoPtr pageInfo);
	static QVariant metaDescription(PageInfoPtr pageInfo);
	static QVariant metaKeywords(PageInfoPtr pageInfo);
	static QVariant firstH1(PageInfoPtr pageInfo);
	static QVariant secondH1(PageInfoPtr pageInfo);
	static QVariant firstH2(PageInfoPtr pageInfo);
	static QVariant secondH2(PageInfoPtr pageInfo);
	static QVariant canonicalLinkElement(PageInfoPtr pageInfo);
	static QVariant statusCode(PageInfoPtr pageInfo);
	static QVariant titleLength(PageInfoPtr pageInfo);
	static QVariant metaDescriptionLength(PageInfoPtr pageInfo);
	static QVariant metaKeywordsLength(PageInfoPtr pageInfo);
	static QVariant firstH1Length(PageInfoPtr pageInfo);
	static QVariant secondH1Length(PageInfoPtr pageInfo);
	static QVariant firstH2Length(PageInfoPtr pageInfo);
	static QVariant secondH2Length(PageInfoPtr pageInfo);
	static QVariant pageSizeBytes(PageInfoPtr pageInfo);
	static QVariant wordCount(PageInfoPtr pageInfo);

	static void checkInfoItem(ItemInfo item);
};

}