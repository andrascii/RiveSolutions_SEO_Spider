#pragma once

namespace QuickieWebBot
{

class PageInfo
{
public:
	enum ItemType : std::uint64_t
	{
		PageInfoItemTypeStart,
		// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

		UrlItemType						= 1 << 0,
		TitleItemType					= 1 << 1,
		ContentItemType					= 1 << 2,
		MetaRefreshItemType				= 1 << 3,
		MetaRobotsItemType				= 1 << 4,
		MetaDescriptionItemType			= 1 << 5,
		MetaKeywordsItemType			= 1 << 6,
		RedirectedUrlItemType			= 1 << 7,
		ServerResponseItemType			= 1 << 8,
		FirstH1ItemType					= 1 << 9,
		SecondH1ItemType				= 1 << 10,
		FirstH2ItemType					= 1 << 11,
		SecondH2ItemType				= 1 << 12,
		CanonicalLinkElementItemType	= 1 << 13,
		StatusCodeItemType				= 1 << 14,
		PageSizeKbItemType				= 1 << 15,
		WordCountItemType				= 1 << 16,
		PageHashItemType				= 1 << 17,
		UrlLengthItemType				= 1 << 18,
		TitleLengthItemType				= 1 << 19,
		MetaDescriptionLengthItemType	= 1 << 20,
		MetaKeywordsLengthItemType		= 1 << 21,
		FirstH1LengthItemType			= 1 << 22,
		SecondH1LengthItemType			= 1 << 23,
		FirstH2LengthItemType			= 1 << 24,
		SecondH2LengthItemType			= 1 << 25,
		AltTextItemType					= 1 << 26,
		AltTextLengthItemType			= 1 << 27,
		ImageSizeKbItemType				= 1 << 28,

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
