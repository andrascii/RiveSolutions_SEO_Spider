#pragma once

#include "model_data_accessor_all_items.h"

namespace QuickieWebBot
{

class ModelDataAccessorOneItem : public ModelDataAccessorAllItems
{
public:
	ModelDataAccessorOneItem(DataCollection::StorageType storageType, int row);

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
	virtual IGridViewResizeStrategy* resizeStrategy() const override;

protected:
	virtual void onModelDataRowAddedInternal(int row, int type) override;

private:
	int m_row;
};

}