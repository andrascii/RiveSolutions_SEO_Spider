#pragma once

#include "model_data_accessor_all_items.h"

namespace QuickieWebBot
{

// class ModelDataAccessorOneItem : public ModelDataAccessorAllItems
// {
// public:
// 	ModelDataAccessorOneItem(DataCollection::StorageType storageType, int row);
// 
// 	virtual QList<PageInfo::ItemType> supportedColumns() const override;
// 	virtual int itemCount() const override;
// 
// 	virtual QVariant item(const QModelIndex& index) const override;
// 	virtual QVariant item(int row, int column) const override;
// 
// 	virtual PageInfoPtr pageInfoAtRow(int row) const override;
// 
// 	virtual IGridViewResizeStrategy* resizeStrategy() const override;
// 
// protected:
// 	virtual void onModelDataRowAddedInternal(int row, int type) override;
// 
// private:
// 	int m_row;
// };

}