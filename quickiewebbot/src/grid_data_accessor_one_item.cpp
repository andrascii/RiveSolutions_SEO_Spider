#include "grid_data_accessor_one_item.h"
#include "grid_view_full_size_resize_strategy.h"

namespace QuickieWebBot
{

// ModelDataAccessorOneItem::ModelDataAccessorOneItem(DataCollection::StorageType storageType, int row)
// 	: GridDataAccessorAllItems(storageType, {})
// 	, m_row(row)
// {
// }
// 
// int ModelDataAccessorOneItem::itemCount() const
// {
// 	return m_columns.size();
// }
// 
// QVariant ModelDataAccessorOneItem::item(const QModelIndex& index) const
// {
// 	return item(index.row(), index.column());
// }
// 
// QVariant ModelDataAccessorOneItem::item(int row, int column) const
// {
// 	if (column == 0)
// 	{
// 		return GridDataAccessorAllItems::headerData(row, Qt::Horizontal);
// 	}
// 
// 	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);
// 
// 	PageInfo::ItemType info = m_columns[row];
// 
// 	return storage[m_row]->itemValue(info);
// }
// 
// PageInfoPtr ModelDataAccessorOneItem::pageInfoAtRow(int row) const
// {
// 	//
// 	// Because this accessor type refers to only one PageInfo
// 	//
// 	Q_UNUSED(row);
// 
// 	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);
// 
// 	return storage[m_row];
// }
// 
// IGridViewResizeStrategy* ModelDataAccessorOneItem::resizeStrategy() const
// {
// 	static GridViewFullSizeResizeStrategy s_resizeStrategy({ 35, 65 });
// 	return &s_resizeStrategy;
// }
// 
// void ModelDataAccessorOneItem::onModelDataRowAddedInternal(int row, int type)
// {
// }

}