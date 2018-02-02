#pragma once

#include "storage_adapter_type.h"
#include "data_collection_groups_factory.h"
#include "imenu_data_provider.h"

namespace SeoSpider
{

class ISummaryDataAccessor : public IMenuDataProvider
{
public:
	virtual ~ISummaryDataAccessor() = default;

	virtual StorageAdapterType itemCategory(const QModelIndex& index) const noexcept = 0;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const noexcept = 0;
	virtual bool isHeaderRow(int row) const noexcept = 0;
	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept = 0;
	virtual QVariant item(const QModelIndex& index) const noexcept = 0;
	virtual QSize span(const QModelIndex& index) const noexcept = 0;
	virtual int columnCount() const noexcept = 0;
	virtual int rowCount() const noexcept = 0;
	virtual void addGroup(AuditGroup group) noexcept = 0;
	virtual const CrawlerEngine::SequencedDataCollection* sequencedDataCollection() const noexcept = 0;
	virtual QObject* qobject() noexcept = 0;
	virtual const DCStorageDescription* storageDescriptionByRow(int row) const noexcept = 0;
	virtual const DCStorageGroupDescription* storageGroupDescriptionByRow(int row) const noexcept = 0;
	virtual const DCStorageDescription* storageDescription(CrawlerEngine::StorageType type) const noexcept = 0;
	virtual const DCStorageGroupDescription* storageGroupDescription(AuditGroup group) const noexcept = 0;

	// signals
	virtual void dataChanged(int row, int column, Qt::ItemDataRole role) const = 0;
	virtual void beginClearData() const = 0;
	virtual void endClearData() const = 0;
};

}