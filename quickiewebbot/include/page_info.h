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
		TitleLengthItemType,
		MetaDescriptionLengthItemType,
		MetaKeywordsLengthItemType,
		FirstH1LengthItemType,
		SecondH1LengthItemType,
		FirstH2LengthItemType,
		SecondH2LengthItemType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		PageInfoItemTypeLast
	};

	static QString itemTitle(ItemType item);
	static QVariant itemValue(const std::shared_ptr<PageInfo>& pageInfo, ItemType item);

	QUrl url;
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
	int statusCode;
	int pageSizeKb;
	int wordCount;
	size_t pageHash;

private:
	using MethodAcceptor = QVariant(*)(const std::shared_ptr<PageInfo>&);

	static MethodAcceptor acceptItem(ItemType item);
	static QVariant acceptUrl(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptContent(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptTitle(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptMetaRefresh(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptMetaRobots(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptRedirectedUrl(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptServerResponse(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptMetaDescription(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptMetaKeywords(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptFirstH1(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptSecondH1(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptFirstH2(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptSecondH2(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptCanonicalLinkElement(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptStatusCode(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptTitleLength(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptMetaDescriptionLength(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptMetaKeywordsLength(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptFirstH1Length(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptSecondH1Length(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptFirstH2Length(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptSecondH2Length(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptPageSizeKb(const std::shared_ptr<PageInfo>& pageInfo);
	static QVariant acceptWordCount(const std::shared_ptr<PageInfo>& pageInfo);

	static void checkInfoItem(PageInfo::ItemType item);
};

using PageInfoPtr = std::shared_ptr<PageInfo>;

Q_DECLARE_METATYPE(PageInfoPtr);

}
