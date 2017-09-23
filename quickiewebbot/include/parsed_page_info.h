#pragma once

#include "parsed_page.h"

namespace QuickieWebBot
{

class ParsedPageInfo
{
public:
	enum Column
	{
		BeginEnumPageInfoItemType,
		// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

		UrlItemType,
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

	ParsedPageInfo(WebCrawler::ParsedPagePtr pageRawPtr);

	static QString itemTypeDescription(Column column);
	static int columnPrefferedSize(Column column);

	QVariant itemValue(Column column) const;

	size_t countLinksFromThisPage() const noexcept;
	WebCrawler::ParsedPageWeakPtr linkFromThisPage(size_t number);

	size_t countLinksToThisPage() const noexcept;
	WebCrawler::ParsedPageWeakPtr linkToThisPage(size_t number);

private:
	using MethodAcceptor = QVariant(ParsedPageInfo::*)() const;

	MethodAcceptor acceptItem(Column item) const;
	QVariant acceptUrl() const;
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

	static void checkColumnType(ParsedPageInfo::Column column);

private:
	WebCrawler::ParsedPagePtr m_pageRawPtr;
};

using PageRawInfoPtr = std::shared_ptr<ParsedPageInfo>;

Q_DECLARE_METATYPE(PageRawInfoPtr);

}
