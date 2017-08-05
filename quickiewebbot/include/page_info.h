#pragma once

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
		UrlLengthItemType,
		TitleLengthItemType,
		MetaDescriptionLengthItemType,
		MetaKeywordsLengthItemType,
		FirstH1LengthItemType,
		SecondH1LengthItemType,
		FirstH2LengthItemType,
		SecondH2LengthItemType,
		ImageAltTextItemType, // remove?
		ImageAltTextLengthItemType, // remove ?
		ImageSizeKbItemType, // remove ?
		ImageCountItemType,
		HasSeveralTitleTagsItemType,
		HasSeveralMetaDescriptionTagsItemType,
		HasSeveralMetaKeywordsTagsItemType,
		HasSeveralH1TagsItemType,
		HasSeveralH2TagsItemType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		EndEnumPageInfoItemType
	};

	static QString itemTypeDescription(ItemType item);
	static int columnPrefferedSize(ItemType item);

	QVariant itemValue(ItemType item, int itemChildIndex = 0);
	void setItemValue(const QVariant& value, ItemType item);

private:
	void setUrl(const QVariant& value);
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
	using MethodAcceptor = QVariant(PageInfo::*)(int);

	MethodAcceptor acceptItem(ItemType item);
	QVariant acceptUrl(int);
	QVariant acceptContent(int);
	QVariant acceptTitle(int);
	QVariant acceptMetaRefresh(int);
	QVariant acceptMetaRobots(int);
	QVariant acceptRedirectedUrl(int);
	QVariant acceptServerResponse(int);
	QVariant acceptMetaDescription(int);
	QVariant acceptMetaKeywords(int);
	QVariant acceptFirstH1(int);
	QVariant acceptSecondH1(int);
	QVariant acceptFirstH2(int);
	QVariant acceptSecondH2(int);
	QVariant acceptCanonicalLinkElement(int);
	QVariant acceptStatusCode(int);
	QVariant acceptUrlLength(int);
	QVariant acceptTitleLength(int);
	QVariant acceptMetaDescriptionLength(int);
	QVariant acceptMetaKeywordsLength(int);
	QVariant acceptFirstH1Length(int);
	QVariant acceptSecondH1Length(int);
	QVariant acceptFirstH2Length(int);
	QVariant acceptSecondH2Length(int);
	QVariant acceptPageSizeKb(int);
	QVariant acceptWordCount(int);
	QVariant acceptPageHash(int);
	QVariant acceptHasSeveralTitles(int);
	QVariant acceptHasSeveralMetaDescriptions(int);
	QVariant acceptHasSeveralMetaKeywords(int);
	QVariant acceptHasSeveralH1(int);
	QVariant acceptHasSeveralH2(int);
	QVariant acceptImageCount(int);
	QVariant acceptImageSizeKb(int childItem);
	QVariant acceptImageAltText(int childItem);
	QVariant acceptImageAltTextLength(int childItem);

	static void checkInfoItem(PageInfo::ItemType item);

private:
	QUrl m_url;
	QString m_title;
	QString m_content;
	QString m_metaRefresh;
	QString m_metaRobots;
	QString m_metaDescription;
	QString m_metaKeywords;
	QString m_redirectedUrl;
	QString m_serverResponse;
	QString m_firstH1;
	QString m_secondH1;
	QString m_firstH2;
	QString m_secondH2;
	QString m_canonicalLinkElement;
	int m_statusCode;
	int m_pageSizeKb;
	int m_wordCount;
	size_t m_pageHash;
};

using PageInfoPtr = std::shared_ptr<PageInfo>;

Q_DECLARE_METATYPE(PageInfoPtr);

}
