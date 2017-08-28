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
	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual QSize span(const QModelIndex& index) const noexcept override;

	virtual int columnCount() const noexcept override;
	virtual int rowCount() const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void dataChanged(int row, int column, Qt::ItemDataRole role) const override;

private:
	ItemStatus itemStatus(int row) const noexcept;

	int rowByStorageType(WebCrawler::DataCollection::StorageType storageType) const noexcept;
	WebCrawler::DataCollection::GuiStorageTypePtr storageByRow(int row) const noexcept;
	void changeItemPixmapIfNeeded(int row) noexcept;

	void initializePixmaps();

	Q_SLOT void emitDataChanged(int row, int storageType);

private:
	static constexpr int s_summaryColumnCount = 2;

	WebCrawler::DataCollection* m_dataCollection;

	QVector<DCStorageDescriptionPtr> m_allGroupRows;
	QMap<int, DCStorageDescriptionPtr> m_groupRows;
	QMap<int, QPair<QPixmap*, DCStorageDescription*>> m_itemRows;

	QPixmap m_okPixmap;
	QPixmap m_warningPixmap;
	QPixmap m_errorPixmap;
};

}