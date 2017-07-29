#pragma once

#include "model_data_accessor_base.h"
#include "data_collection.h"
#include "grid_view_resize_strategy.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

class GridViewResizeStrategy;

class ModelDataAccessorAllItems 
	: public QObject
	, public ModelDataAccessorBase
{
	Q_OBJECT

public:
	ModelDataAccessorAllItems(DataCollection::StorageType storageType, QVector<PageInfo::ItemType> columns);

	virtual int columnCount() const override;
	virtual int rowCount() const override;

	virtual QString headerData(int column, Qt::Orientation orientation) const override;

	virtual QVariant itemValue(const QModelIndex& index) const override;
	virtual QVariant itemValue(int row, int column) const override;

	virtual PageInfoPtr pageInfoAtRow(int row) const override;

	virtual QColor itemBackgroundColor(const QModelIndex& index) const override;
	virtual int itemColSpan(const QModelIndex& index) const override;
	virtual int flags(const QModelIndex& index) const override;
	virtual QPixmap* pixmap(const QModelIndex& index) const override;
	virtual QObject* qobject() override;

	virtual IGridViewResizeStrategy* resizeStrategy() const override;
	virtual ModelDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;

	virtual std::vector<const GridViewPainter*> painters(const QModelIndex& index) const override;
	virtual std::vector<GridViewPainter*> painters(const QModelIndex& index) override;

	virtual const GridViewPainter* backgroundPainter(const QModelIndex& index) const override;
	virtual GridViewPainter* backgroundPainter(const QModelIndex& index) override;

	virtual const GridViewPainter* textPainter(const QModelIndex& index) const override;
	virtual GridViewPainter* textPainter(const QModelIndex& index) override;

	Q_SIGNAL virtual void itemChanged(int row, int column) override;
	Q_SIGNAL virtual void rowRemoved(int row) override;
	Q_SIGNAL virtual void rowAdded(int row) override;
	Q_SIGNAL virtual void reset() override;

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
};

}