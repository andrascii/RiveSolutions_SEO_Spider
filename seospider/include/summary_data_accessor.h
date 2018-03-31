#pragma once

#include "isummary_data_accessor.h"
#include "data_collection_groups_factory.h"

namespace SeoSpider
{

class SummaryDataSet;

class SummaryDataAccessor : public QObject, public ISummaryDataAccessor
{
	Q_OBJECT

public:
	SummaryDataAccessor(SummaryDataSet* dataSet);

	virtual void setSortableDataSet(SummaryDataSet* dataSet) noexcept override;
	virtual void enableSortableDataSet() noexcept override;
	virtual void enablePlainDataSet() noexcept override;
	virtual bool hasSortableDataSet() const noexcept override;
	virtual void selectRow(int row) noexcept override;
	virtual int selectedRow() const noexcept override;
	virtual StorageAdapterType itemCategory(const QModelIndex& index) const noexcept override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const noexcept override;
	virtual bool isHeaderRow(int row) const noexcept override;
	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual QSize span(const QModelIndex& index) const noexcept override;
	virtual int columnCount() const noexcept override;
	virtual int rowCount() const noexcept override;
	virtual void addGroup(AuditGroup group) noexcept override;
	virtual const CrawlerEngine::SequencedDataCollection* sequencedDataCollection() const noexcept override;
	virtual QObject* qobject() noexcept override;
	virtual const DCStorageDescription* storageDescriptionByRow(int row) const noexcept override;
	virtual const DCStorageGroupDescription* storageGroupDescriptionByRow(int row) const noexcept override;
	virtual const DCStorageDescription* storageDescription(CrawlerEngine::StorageType type) const noexcept override;
	virtual const DCStorageGroupDescription* storageGroupDescription(AuditGroup group) const noexcept override;
	virtual Menu menuFor(const QModelIndex& index) const override;
	virtual int rowByStorageType(CrawlerEngine::StorageType storageType) const noexcept override;

signals:
	virtual void dataChanged(int row, int column, Qt::ItemDataRole role) const override;
	virtual void beginClearData() const override;
	virtual void endClearData() const override;
	virtual void rowSelected(int row) override;
	virtual void dataSetChanged() const override;

private slots:
	void emitDataChanged(int, CrawlerEngine::StorageType);
	void validateSelectedRow();

private:
	SummaryDataSet* m_currentDataSet;
	SummaryDataSet* m_dataSet;
	SummaryDataSet* m_sortableDataSet;
	std::pair<int, CrawlerEngine::StorageType> m_selectedRow;
};

}