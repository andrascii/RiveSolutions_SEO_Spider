#pragma once

#include "page_raw.h"

namespace QuickieWebBot
{

class PageRawInfo
{
public:
	enum Column
	{
		BeginEnumPageInfoItemType,
		// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

		UrlItemType,
		FromUrlItemType,
		TitleItemType,
		ContentTypeItemType,
		MetaRefreshItemType,
		MetaRobotsItemType,
		MetaDescriptionItemType,
		MetaKeywordsItemType,
		RedirectedUrlItemType,
		ServerResponseItemType,
		AltTextItemType,

		FirstH1ItemType,
		SecondH1ItemType,
		FirstH2ItemType,
		SecondH2ItemType,

		CanonicalLinkElementItemType,
		StatusCodeItemType,
		PageSizeKbItemType,
		WordCountItemType,
		PageHashItemType,

		ImageSizeKbItemType,

		NoFollowDoFollowLinkItemType,
		AltTextLinkItemType,

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
		AltTextLengthItemType,

		// !!!!!!!!!!!!!!!!!!! add new length enumerators above this!!!!!!!!!!!!!!!!!!!
		EndLengthEnumeratorsItemType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		EndEnumPageInfoItemType
	};

	PageRawInfo(WebCrawler::PageRawPtr pageRawPtr);

	static QString itemTypeDescription(Column column);
	static int columnPrefferedSize(Column column);

	QVariant itemValue(Column column) const;

	size_t countLinksFromThisPage() const noexcept;
	WebCrawler::PageRawWeakPtr linkFromThisPage(size_t number);

	size_t countLinksToThisPage() const noexcept;
	WebCrawler::PageRawWeakPtr linkToThisPage(size_t number);

private:
	using MethodAcceptor = QVariant(PageRawInfo::*)() const;

	MethodAcceptor acceptItem(Column item) const;
	QVariant acceptUrl() const;
	QVariant acceptFromUrl() const;
	QVariant acceptContentType() const;
	QVariant acceptTitle() const;
	QVariant acceptMetaRefresh() const;
	QVariant acceptMetaRobots() const;
	QVariant acceptRedirectedUrl() const;
	QVariant acceptServerResponse() const;
	QVariant acceptMetaDescription() const;
	QVariant acceptMetaKeywords() const;
	QVariant acceptFirstH1() const;
	QVariant acceptSecondH1() const;
	QVariant acceptFirstH2() const;
	QVariant acceptSecondH2() const;
	QVariant acceptCanonicalLinkElement() const;
	QVariant acceptStatusCode() const;
	QVariant acceptUrlLength() const;
	QVariant acceptTitleLength() const;
	QVariant acceptMetaDescriptionLength() const;
	QVariant acceptMetaKeywordsLength() const;
	QVariant acceptFirstH1Length() const;
	QVariant acceptSecondH1Length() const;
	QVariant acceptFirstH2Length() const;
	QVariant acceptSecondH2Length() const;
	QVariant acceptAltTextLength() const;
	QVariant acceptPageSizeKb() const;
	QVariant acceptWordCount() const;
	QVariant acceptPageHash() const;
	QVariant acceptImageSizeKb() const;
	QVariant acceptAltText() const;
	QVariant acceptDofollowNofollow() const;

	static void checkColumnType(PageRawInfo::Column column);

private:
	WebCrawler::PageRawPtr m_pageRawPtr;
};

using PageRawInfoPtr = std::shared_ptr<PageRawInfo>;

Q_DECLARE_METATYPE(PageRawInfoPtr);

}
