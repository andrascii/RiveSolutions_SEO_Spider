#pragma once

#include "parsed_page_info.h"
#include "parsed_page_info_storage_adapter.h"
#include "sequenced_data_collection_types.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

class PageLinksStorageAdapter : public QObject, public IPageLinksStorageAdapter
{
	Q_OBJECT

public:
	PageLinksStorageAdapter(ParsedPageInfoPtr associatedParsedPage, PageLinkContext context);

	virtual void setAvailableColumns(QList<ParsedPageInfo::PageLinksColumn> availableColumns) noexcept override;

	virtual QList<ParsedPageInfo::PageLinksColumn> availableColumns() const noexcept override;

	virtual QString columnDescription(int columnIndex) const noexcept override;


	virtual int columnWidth(int columnNumber) const noexcept override;

	virtual int columnCount() const noexcept override;


	virtual int itemCount() const noexcept override;

	virtual QVariant item(const QModelIndex& index) const noexcept override;

	virtual ItemType itemType(const QModelIndex& index) const noexcept override;


	virtual ParsedPageInfoPtr parsedPageInfoPtr(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

signals:
	virtual void parsedPageInfoAdded(int rowIndex) const override;

	virtual void beginClearData() const override;

	virtual void endClearData() const override;

private slots:
	void onBeginClearData();

	void onParsedPageAdded(LinksToThisResourceChanges changes);

private:
	ParsedPageInfoPtr m_parsedPageInfo; 

	PageLinkContext m_context;

	QList<ParsedPageInfo::PageLinksColumn> m_availableColumns;
};

}