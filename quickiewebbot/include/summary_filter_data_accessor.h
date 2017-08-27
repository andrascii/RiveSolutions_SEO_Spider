#pragma once

#include "isummary_data_accessor.h"
#include "data_collection_groups_factory.h"

namespace QuickieWebBot
{

class SummaryFilterDataAccessor 
	: public QObject
	, public ISummaryDataAccessor
{
	Q_OBJECT

private:
	enum ItemStatus
	{
		StatusOK,
		StatusWarning,
		StatusError
	};

public:
	SummaryFilterDataAccessor(WebCrawler::DataCollection* dataCollection);

	virtual SummaryCategoryItem itemCategory(const QModelIndex& index) const noexcept override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const noexcept override;

	virtual bool isHeaderItem(const QModelIndex& index) const noexcept override;
	virtual QPixmap pixmap(const QModelIndex& index) const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual QSize span(const QModelIndex& index) const noexcept override;

	virtual int columnCount() const noexcept override;
	virtual int rowCount() const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void dataChanged(int row, int column) override;

private:
	ItemStatus itemStatus(const QModelIndex& index) const noexcept;
	int rowByStorageType(WebCrawler::DataCollection::StorageType storageType) const noexcept;

	WebCrawler::DataCollection::GuiStorageTypePtr storageByRow(int row) const noexcept;

	Q_SLOT void emitDataChanged(int row, int storageType);

private:
	static constexpr int s_summaryColumnCount = 2;

	WebCrawler::DataCollection* m_dataCollection;

	QVector<DCStorageDescriptionPtr> m_allGroupRows;

	QMap<int, DCStorageDescriptionPtr> m_groupRows;

	QMap<int, DCStorageDescription*> m_itemRows;
};

}