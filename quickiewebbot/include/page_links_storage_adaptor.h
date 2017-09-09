#pragma once

#include "page_raw_info.h"
#include "page_raw_info_storage_adaptor.h"

namespace QuickieWebBot
{

enum class PageLinkType
{
	LinkFromThisPageType,
	LinkToThisPageType,
	ImageFromThisPageType
};

class PageLinksStorageAdaptor : public QObject, public IStorageAdaptor
{
	Q_OBJECT

public:
	PageLinksStorageAdaptor(PageRawInfoPtr associatedPageRaw, PageLinkType pageLinkType);

	virtual void setAvailableColumns(QList<PageRawInfo::Column> availableColumns) noexcept override;
	virtual QList<PageRawInfo::Column> availableColumns() const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;

	virtual int itemCount() const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual PageRawInfo::Column itemType(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void pageRawInfoAdded(int rowIndex) const override;

private:
	using PageRawInfoCountAcceptorMethodType = size_t(PageRawInfo::*)() const;
	using PageRawInfoLinkAcceptorMethodType = WebCrawler::PageRawWeakPtr(PageRawInfo::*)(size_t) ;

	PageRawInfoCountAcceptorMethodType countLinks() const noexcept;
	PageRawInfoLinkAcceptorMethodType link() const noexcept;

private:
	PageRawInfoPtr m_associatedPageRawInfoPtr; 
	PageLinkType m_pageLinkType;

	QList<PageRawInfo::Column> m_availableColumns;
};

}