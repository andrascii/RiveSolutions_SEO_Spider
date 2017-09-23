#pragma once

#include "parsed_page_info.h"
#include "parsed_page_info_storage_adaptor.h"

namespace QuickieWebBot
{

enum class PageLinkType
{
	LinksOnThisPageType,
	LinksToThisPageType,
	ImagesOnThisPageType
};

class PageLinksStorageAdaptor : public QObject, public IStorageAdaptor
{
	Q_OBJECT

public:
	PageLinksStorageAdaptor(PageRawInfoPtr associatedPageRaw, PageLinkType pageLinkType);

	virtual void setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept override;
	virtual QList<ParsedPageInfo::Column> availableColumns() const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;

	virtual int itemCount() const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual ParsedPageInfo::Column itemType(const QModelIndex& index) const noexcept override;

	virtual PageRawInfoPtr pageRawInfoPtr(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void pageRawInfoAdded(int rowIndex) const override;

private:
	using PageRawInfoCountAcceptorMethodType = size_t(ParsedPageInfo::*)() const;
	using PageRawInfoLinkAcceptorMethodType = WebCrawler::ParsedPageWeakPtr(ParsedPageInfo::*)(size_t) ;

	PageRawInfoCountAcceptorMethodType countLinks() const noexcept;
	PageRawInfoLinkAcceptorMethodType link() const noexcept;

private:
	PageRawInfoPtr m_associatedPageRawInfoPtr; 
	PageLinkType m_pageLinkType;

	QList<ParsedPageInfo::Column> m_availableColumns;
};

}