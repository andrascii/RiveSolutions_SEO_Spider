#include "model_data_accessor_one_item.h"
#include "grid_view_full_size_resize_strategy.h"

namespace QuickieWebBot
{

ModelDataAccessorOneItem::ModelDataAccessorOneItem(DataCollection::StorageType storageType, int row)
	: ModelDataAccessorAllItems(storageType, {})
	, m_row(row)
{
}

int ModelDataAccessorOneItem::columnCount() const
{
	return 2;
}

QString ModelDataAccessorOneItem::columnText(int column) const
{
	return QString();
	//return ModelDataAccessorAllItems::columnText(column);
}

int ModelDataAccessorOneItem::rowCount() const
{
	return m_columns.size();
}

QVariant ModelDataAccessorOneItem::itemValue(const QModelIndex& index) const
{
	return itemValue(index.row(), index.column());
}

QVariant ModelDataAccessorOneItem::itemValue(int row, int column) const
{
	if (column == 0)
	{
		return ModelDataAccessorAllItems::columnText(row);
	}

	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);

	PageInfo::ItemType info = m_columns[row];

	return storage[m_row]->itemValue(info);
}

PageInfoPtr ModelDataAccessorOneItem::pageInfoAtRow(int row) const
{
	//
	// Because this accessor type refers to only one PageInfo
	//
	Q_UNUSED(row);

	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);

	return storage[m_row];
}

QColor ModelDataAccessorOneItem::itemBackgroundColor(const QModelIndex& index) const
{
	return QColor();
}

int ModelDataAccessorOneItem::itemColSpan(const QModelIndex& index) const
{
	return 1;
}

int ModelDataAccessorOneItem::flags(const QModelIndex& index) const
{	
	if (index.column() == 0)
	{
		return ItemFlagNone;
	}
	
	return m_columns[index.row()] == PageInfo::UrlItemType ? ItemFlagUrl : ItemFlagNone;
}

QPixmap* ModelDataAccessorOneItem::pixmap(const QModelIndex& index) const
{
	return nullptr;
}

IGridViewResizeStrategy* ModelDataAccessorOneItem::resizeStrategy() const
{
	static GridViewFullSizeResizeStrategy s_resizeStrategy({ 35, 65 });
	return &s_resizeStrategy;
}

void ModelDataAccessorOneItem::onModelDataRowAddedInternal(int row, int type)
{
	Q_UNUSED(row);
	Q_UNUSED(type);
}

}