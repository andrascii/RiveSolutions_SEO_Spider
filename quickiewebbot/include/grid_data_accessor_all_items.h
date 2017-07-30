#pragma once

#include "data_collection.h"
#include "grid_data_accessor_base.h"
#include "grid_view_resize_strategy.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

class GridViewResizeStrategy;

class GridDataAccessorAllItems 
	: public QObject
	, public GridDataAccessorBase
{
	Q_OBJECT

public:
	GridDataAccessorAllItems(DataCollection::StorageType storageType, QVector<PageInfo::ItemType> columns);

	virtual int columnsCount() const override;
	virtual int itemCount() const override;

	virtual QVariant columnDescription(int section) const override;

	virtual QVariant item(const QModelIndex& index) const override;
	virtual QVariant item(int row, int column) const override;

	virtual PageInfoPtr pageInfoAtRow(int row) const override;

	virtual IGridViewResizeStrategy* resizeStrategy() const override;
	virtual GridDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;

	virtual QObject* qobject() override;

signals:
	virtual void itemChanged(int row, int column) override;
	virtual void rowRemoved(int row) override;
	virtual void rowAdded(int row) override;
	virtual void reset() override;

protected:
	Q_SLOT void onModelDataRowAdded(int row, int type);
	virtual void onModelDataRowAddedInternal(int row, int type);

protected:
	const DataCollection* m_modelControllerData;
	DataCollection::StorageType m_storageType;
	QVector<PageInfo::ItemType> m_columns;

	std::unique_ptr<GridViewResizeStrategy> m_resizeStrategy;

	GridViewPainterText m_painterText;
	GridViewPainterBackground m_painterBackground;

	QList<PageInfo::ItemType> m_supportedColumns;
};

}