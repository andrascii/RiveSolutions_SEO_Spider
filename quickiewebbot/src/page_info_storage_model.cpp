#include "page_info_storage_model.h"
#include "grid_view_resize_strategy.h"

namespace QuickieWebBot
{

PageInfoStorageModel::PageInfoStorageModel(QObject* parent)
	: IGridModel(parent)
	, m_resizeStrategy(std::make_unique<GridViewResizeStrategy>())
{
}

Qt::ItemFlags PageInfoStorageModel::flags(const QModelIndex& index) const
{
	return Qt::NoItemFlags;
}

IGridViewResizeStrategy* PageInfoStorageModel::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

QVariant PageInfoStorageModel::data(const QModelIndex& index, int role) const
{
	return QVariant();
}

QVariant PageInfoStorageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

int PageInfoStorageModel::columnCount(const QModelIndex&) const
{
	return 0;
}

int PageInfoStorageModel::rowCount(const QModelIndex& parent) const
{
	return 0;
}

}