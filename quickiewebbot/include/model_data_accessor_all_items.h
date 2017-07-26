#pragma once

#include "model_data_accessor_base.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class GridViewResizeStrategy;

class ModelDataAccessorAllItems 
	: public QObject
	, public ModelDataAccessorBase
{
	Q_OBJECT

public:
	ModelDataAccessorAllItems(DataCollection::StorageType storageType);
	~ModelDataAccessorAllItems();
	virtual int columnCount() const override;
	virtual QString columnText(int column) const override;

	virtual int rowCount() const override;
	virtual QVariant itemValue(const QModelIndex& index) const override;
	virtual QColor itemBackgroundColor(const QModelIndex& index) const override;
	virtual int itemColSpan(const QModelIndex& index) const override;
	virtual int flags(const QModelIndex& index) const override;

	virtual QPixmap* pixmap(const QModelIndex& index) const override;

	virtual QObject* qobject() override;

	// signals
	Q_SIGNAL virtual void itemChanged(int row, int column) override;
	Q_SIGNAL virtual void rowRemoved(int row) override;
	Q_SIGNAL virtual void rowAdded(int row) override;
	Q_SIGNAL virtual void reset() override;

	virtual std::vector<GridViewPainter*> painters(const QModelIndex& index) const override;
	virtual IGridViewResizeStrategy* resizeStrategy() const override;

	virtual std::unique_ptr<ModelDataAccessorFactoryParams> childViewParams(const QItemSelection& selection) const override;

protected:
	Q_SLOT void onModelDataRowAdded(int row, int type);
	virtual void onModelDataRowAddedInternal(int row, int type);

protected:
	const DataCollection* m_modelControllerData;
	DataCollection::StorageType m_storageType;
	QVector<int> m_columns;
	std::unique_ptr<GridViewResizeStrategy> m_resizeStrategy;
};

}