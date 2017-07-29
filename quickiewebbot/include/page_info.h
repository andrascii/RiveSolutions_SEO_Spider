#pragma once

namespace QuickieWebBot
{

struct PageInfo
{
	enum ItemType
	{
		PageInfoItemTypeStart,
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
		AltTextItemType,
		AltTextLengthItemType,
		ImageSizeKbItemType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		PageInfoItemTypeLast
	};

	static QString itemTitle(ItemType item);
	static int columnPrefferedSize(ItemType item);

	QVariant itemValue(ItemType item);
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
	using MethodAcceptor = QVariant(PageInfo::*)();

	MethodAcceptor acceptItem(ItemType item);
	QVariant acceptUrl();
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
