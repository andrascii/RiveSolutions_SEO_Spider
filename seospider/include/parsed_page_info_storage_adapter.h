#pragma once

#include "istorage_adapter.h"
#include "parsed_page_info.h"
#include "sequenced_data_collection.h"

namespace SeoSpider
{

class ParsedPageInfoStorageAdapter : public QObject, public IParsedPageStorageAdapter
{
	Q_OBJECT

public:
	ParsedPageInfoStorageAdapter(
		CrawlerEngine::SequencedDataCollection* dataCollection,
		CrawlerEngine::ISequencedStorage* associatedStorage,
		CrawlerEngine::StorageType storageType,
		QObject* parent = nullptr
	);

	virtual void setAvailableColumns(const QVector<ParsedPageInfo::Column>& availableColumns) noexcept override;
	virtual const QVector<ParsedPageInfo::Column>& availableColumns() const noexcept override;
	virtual void setCurrentColumns(const QVector<ParsedPageInfo::Column>& currentColumns) noexcept override;
	virtual const QVector<ParsedPageInfo::Column>& currentColumns() const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;
	virtual int columnWidth(int columnIndex) const noexcept override;
	virtual int columnCount() const noexcept override;
	virtual bool columnEnabled(int columnIndex) const noexcept override;
	virtual int itemCount() const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual ItemType itemType(const QModelIndex& index) const noexcept override;
	virtual RowResourceType resourceType(int row) const noexcept override;
	virtual ParsedPageInfoPtr parsedPageInfoPtr(const QModelIndex& index) const noexcept override;
	virtual const CrawlerEngine::ParsedPage* parsedPage(const QModelIndex& index) const noexcept override;
	virtual Menu menuFor(const QModelIndex& index) const override;
	virtual QObject* qobject() noexcept override;

signals:
	virtual void parsedPageInfoAdded(int rowIndex) const override;
	virtual void parsedPageInfoRemoved(int rowIndex) const override;
	virtual void parsedPageInfoReplaced(int rowIndex) const override;
	virtual void repaintIndicesRange(std::pair<int, int> indicesRange) const override;
	virtual void repaintColumn(int rowIndex) const override;
	virtual void beginClearData() const override;
	virtual void endClearData() const override;
	virtual void customColumnCountChanged() const override;

private slots:
	void onStorageUpdated(int row, CrawlerEngine::StorageType type);
	void onPageRemoved(int row, CrawlerEngine::StorageType type);
	void onPageReplaced(int row, CrawlerEngine::StorageType type);
	void onRepaintIndicesRange(std::pair<int, int> indicesRange, CrawlerEngine::StorageType type);

private:
	CrawlerEngine::SequencedDataCollection* m_dataCollection;
	CrawlerEngine::ISequencedStorage* m_associatedStorage;
	QVector<ParsedPageInfo::Column> m_availableColumns;
	QVector<ParsedPageInfo::Column> m_currentColumns;
	CrawlerEngine::StorageType m_storageType;
	int m_linksToThisPageIndex;
};

}