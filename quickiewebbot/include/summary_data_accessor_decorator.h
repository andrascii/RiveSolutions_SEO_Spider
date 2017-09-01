#pragma once

#include "isummary_data_accessor.h"

namespace QuickieWebBot
{

class SummaryDataAccessorDecorator : public QObject, public ISummaryDataAccessor
{
	Q_OBJECT

public:
	SummaryDataAccessorDecorator(ISummaryDataAccessor* dataAccessor);

	virtual SummaryCategoryItem itemCategory(const QModelIndex& index) const noexcept override;

	virtual Qt::ItemFlags flags(const QModelIndex& index) const noexcept override;
	virtual bool isHeaderRow(int row) const noexcept override;

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;

	virtual QSize span(const QModelIndex& index) const noexcept override;

	virtual int columnCount() const noexcept override;
	virtual int rowCount() const noexcept override;

	virtual void addGroup(AuditGroup group) noexcept override;

	virtual const WebCrawler::DataCollection* dataCollection() const noexcept override;

	virtual QObject* qobject() noexcept override;

	virtual const DCStorageDescription* storageDescriptionByRow(int row) const noexcept override;
	virtual const DCStorageGroupDescription* storageGroupDescriptionByRow(int row) const noexcept override;
	virtual const DCStorageDescription* storageDescription(WebCrawler::DataCollection::StorageType type) const noexcept override;
	virtual const DCStorageGroupDescription* storageGroupDescription(AuditGroup group) const noexcept override;

	// signals

	Q_SIGNAL virtual void dataChanged(int row, int column, Qt::ItemDataRole role) const override;

private:
	std::unique_ptr<ISummaryDataAccessor> m_summaryDataAccessor;
};

}
