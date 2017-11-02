#pragma once

#include "istorage_adaptor.h"
#include "parsed_page_info.h"
#include "sequenced_data_collection.h"

namespace SeoSpider
{

class ParsedPageInfoStorageAdaptor : public QObject, public IParsedPageStorageAdaptor
{
	Q_OBJECT

public:
	ParsedPageInfoStorageAdaptor(const CrawlerEngine::SequencedDataCollection::SequencedStorageTypePtr& associatedStorage,
		CrawlerEngine::StorageType storageType,
		QObject* parent = nullptr);

	virtual void setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept override;
	virtual QList<ParsedPageInfo::Column> availableColumns() const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;

	virtual int columnWidth(int columnNumber) const noexcept override;

	virtual int columnCount() const noexcept override;
	virtual int itemCount() const noexcept override;

	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual ItemType itemType(const QModelIndex& index) const noexcept override;

	virtual ParsedPageInfoPtr parsedPageInfoPtr(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void parsedPageInfoAdded(int rowIndex) const override;

#ifdef QT_DEBUG
	CrawlerEngine::ParsedPage* parsedPage(const QModelIndex& index) const noexcept;
#endif

private:
	Q_SLOT void onStorageUpdated(int row, int type);

private:
	CrawlerEngine::SequencedDataCollection::SequencedStorageTypePtr m_associatedStorage;
	QList<ParsedPageInfo::Column> m_availableColumns;
	CrawlerEngine::StorageType m_storageType;
};

}