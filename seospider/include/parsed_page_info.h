#pragma once

#include "parsed_page.h"

namespace SeoSpider
{

enum class PageLinkContext
{
	LinksOnThisPage,
	LinksToThisPage,
	ImagesOnThisPage
};

class ParsedPageInfo
{
public:
	enum class Column
	{
		BeginEnumPageInfoColumn,
		// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

		UrlColumn,
		TitleColumn,
		ContentTypeColumn,
		MetaRefreshColumn,
		MetaRobotsColumn,
		MetaDescriptionColumn,
		MetaKeywordsColumn,
		RedirectedUrlColumn,
		ServerResponseColumn,

		FirstH1Column,
		SecondH1Column,
		FirstH2Column,
		SecondH2Column,

		CanonicalLinkElementColumn,
		StatusCodeColumn,
		PageSizeKbColumn,
		WordCountColumn,
		PageHashColumn,

		ImageSizeKbColumn,

		BeginLengthEnumeratorsColumn,
		// !!!!!!!!!!!!!!!!!!! add length enumerators below this!!!!!!!!!!!!!!!!!!!

		UrlLengthColumn,
		TitleLengthColumn,
		MetaDescriptionLengthColumn,
		MetaKeywordsLengthColumn,
		FirstH1LengthColumn,
		SecondH1LengthColumn,
		FirstH2LengthColumn,
		SecondH2LengthColumn,

		// !!!!!!!!!!!!!!!!!!! add new length enumerators above this!!!!!!!!!!!!!!!!!!!
		EndLengthEnumeratorsColumn,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		EndEnumPageInfoColumn
	};

	enum class PageLinksColumn
	{
		UrlColumn,
		AltOrTitleColumn,
		AltOrTitleLengthColumn,
		LinkParameterColumn,
		StatusCodeColumn
	};

	ParsedPageInfo(const CrawlerEngine::ParsedPage* parsedPage);

	static QString itemTypeDescription(Column column);
	static QString itemTypeDescription(PageLinksColumn column);
	static int columnPrefferedSize(Column column);
	static int columnPrefferedSize(PageLinksColumn column);

	QVariant itemValue(Column column) const;
	QVariant itemValue(PageLinksColumn pageLinksColumn, PageLinkContext context, size_t number) const;
	size_t itemCount(PageLinkContext context) const;

private:
	using MethodAcceptor = QVariant(ParsedPageInfo::*)() const;
	using PageLinksPointer = std::deque<CrawlerEngine::ResourceLink> CrawlerEngine::ParsedPage::*;

	static bool isPageLinksColumnMappedToParsedPageColumn(PageLinksColumn pageLinksColumn) noexcept;
	static PageLinksPointer pointerByContext(PageLinkContext context);
	static QString linkParameterDescription(CrawlerEngine::LinkParameter linkParameter);
	static Column mapPageLinksColumnToParsedPageColumn(PageLinksColumn pageLinksColumn) noexcept;

	static MethodAcceptor acceptItemMethod(Column item);
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
	QVariant acceptPageSizeKb() const;
	QVariant acceptWordCount() const;
	QVariant acceptPageHash() const;
	QVariant acceptImageSizeKb() const;

	static void checkColumnType(ParsedPageInfo::Column column);

private:
	const CrawlerEngine::ParsedPage* m_parsedPage;
};

using ParsedPageInfoPtr = std::shared_ptr<ParsedPageInfo>;

Q_DECLARE_METATYPE(ParsedPageInfoPtr);

}
