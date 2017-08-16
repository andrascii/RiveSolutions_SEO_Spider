#pragma once

#include "page_raw.h"

namespace QuickieWebBot
{

class PageInfo
{
public:
	enum ItemType : std::uint64_t
	{
		BeginEnumPageInfoItemType,
		// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

		UrlItemType,
		FromUrlItemType,
		TitleItemType,
		ContentItemType,
		MetaRefreshItemType,
		MetaRobotsItemType,
		MetaDescriptionItemType,
		MetaKeywordsItemType,
		RedirectedUrlItemType,
		ServerResponseItemType,

		FirstH1ItemType,
		SecondH1ItemType,
		FirstH2ItemType,
		SecondH2ItemType,

		CanonicalLinkElementItemType,
		StatusCodeItemType,
		PageSizeKbItemType,
		WordCountItemType,
		PageHashItemType,

		BeginLengthEnumeratorsItemType,
		// !!!!!!!!!!!!!!!!!!! add length enumerators below this!!!!!!!!!!!!!!!!!!!

		UrlLengthItemType,
		TitleLengthItemType,
		MetaDescriptionLengthItemType,
		MetaKeywordsLengthItemType,
		FirstH1LengthItemType,
		SecondH1LengthItemType,
		FirstH2LengthItemType,
		SecondH2LengthItemType,

		// !!!!!!!!!!!!!!!!!!! add new length enumerators above this!!!!!!!!!!!!!!!!!!!
		EndLengthEnumeratorsItemType,

		ImageAltTextItemType,
		ImageAltTextLengthItemType,
		ImageSizeKbItemType,

		HasSeveralTitleTagsItemType,
		HasSeveralMetaDescriptionTagsItemType,
		HasSeveralMetaKeywordsTagsItemType,
		HasSeveralH1TagsItemType,
		HasSeveralH2TagsItemType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		EndEnumPageInfoItemType
	};

	PageInfo(WebCrawler::PageRawPtr pageRawPtr);

	static QString itemTypeDescription(ItemType item);
	static int columnPrefferedSize(ItemType item);

	QVariant itemValue(ItemType item);
	void setItemValue(const QVariant& value, ItemType item);

private:
	void setUrl(const QVariant& value);
	void setFromUrl(const QVariant& value);
	void setContent(const QVariant& value);
	void setTitle(const QVariant& value);
	void setMetaRefresh(const QVariant& value);
	void setMetaRobots(const QVariant& value);
	void setRedirectedUrl(const QVariant& value);
	void setServerResponse(const QVariant& value);
	void setMetaDescription(const QVariant& value);
	void setMetaKeywords(const QVariant& value);
	void setFirstH1(const QVariant& value);
	void setSecondH1(const QVariant& value);
	void setFirstH2(const QVariant& value);
	void setSecondH2(const QVariant& value);
	void setCanonicalLinkElement(const QVariant& value);
	void setStatusCode(const QVariant& value);
	void setPageSizeKb(const QVariant& value);
	void setWordCount(const QVariant& value);
	void setPageHash(const QVariant& value);

private:
	using MethodAcceptor = QVariant(PageInfo::*)();

	MethodAcceptor acceptItem(ItemType item);
	QVariant acceptUrl();
	QVariant acceptFromUrl();
	QVariant acceptContent();
	QVariant acceptTitle();
	QVariant acceptMetaRefresh();
	QVariant acceptMetaRobots();
	QVariant acceptRedirectedUrl();
	QVariant acceptServerResponse();
	QVariant acceptMetaDescription();
	QVariant acceptMetaKeywords();
	QVariant acceptFirstH1();
	QVariant acceptSecondH1();
	QVariant acceptFirstH2();
	QVariant acceptSecondH2();
	QVariant acceptCanonicalLinkElement();
	QVariant acceptStatusCode();
	QVariant acceptUrlLength();
	QVariant acceptTitleLength();
	QVariant acceptMetaDescriptionLength();
	QVariant acceptMetaKeywordsLength();
	QVariant acceptFirstH1Length();
	QVariant acceptSecondH1Length();
	QVariant acceptFirstH2Length();
	QVariant acceptSecondH2Length();
	QVariant acceptPageSizeKb();
	QVariant acceptWordCount();
	QVariant acceptPageHash();
	QVariant acceptHasSeveralTitles();
	QVariant acceptHasSeveralMetaDescriptions();
	QVariant acceptHasSeveralMetaKeywords();
	QVariant acceptHasSeveralH1();
	QVariant acceptHasSeveralH2();
	QVariant acceptImageSizeKb();
	QVariant acceptImageAltText();
	QVariant acceptImageAltTextLength();

	static void checkInfoItem(PageInfo::ItemType item);

private:
	WebCrawler::PageRawPtr m_pageRawPtr;
};

using PageInfoPtr = std::shared_ptr<PageInfo>;

Q_DECLARE_METATYPE(PageInfoPtr);

}
