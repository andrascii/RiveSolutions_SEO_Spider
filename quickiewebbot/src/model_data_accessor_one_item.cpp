#include "model_data_accessor_one_item.h"

namespace QuickieWebBot
{



ModelDataAccessorOneItem::ModelDataAccessorOneItem(DataCollection::StorageType storageType, int row)
	: ModelDataAccessorAllItems(storageType)
	, m_row(row)
{
}

int ModelDataAccessorOneItem::columnCount() const
{
	return 1;
}

QString ModelDataAccessorOneItem::columnText(int column) const
{
	return ModelDataAccessorAllItems::columnText(column);
}

int ModelDataAccessorOneItem::rowCount() const
{
	return m_columns.size();
}

QVariant ModelDataAccessorOneItem::itemValue(const QModelIndex& index) const
{
	const DataCollection::GuiStorageType* storage = m_modelControllerData->guiStorage(m_storageType);
	PageInfo::ItemType info = static_cast<PageInfo::ItemType>(m_columns[index.row()]);

	return (*storage)[m_row]->itemValue(info);
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
	return 0;
}

QPixmap* ModelDataAccessorOneItem::pixmap(const QModelIndex& index) const
{
	return nullptr;
}

void ModelDataAccessorOneItem::onModelDataRowAddedInternal(int row, int type)
{
	Q_UNUSED(row);
	Q_UNUSED(type);
}

}