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
	if (!m_storageAdaptor)
	{
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PageInfoStorageModel::setStorageAdaptor(DataCollection::StorageAdaptor storageAdaptor) noexcept
{
	m_storageAdaptor = storageAdaptor;
}

IGridViewResizeStrategy* PageInfoStorageModel::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

const DataCollection::StorageAdaptor* PageInfoStorageModel::storageAdaptor() const
{
	try
	{
		return std::addressof(m_storageAdaptor.value());
	}
	catch (const std::bad_optional_access&)
	{
		return nullptr;
	}
}

DataCollection::StorageAdaptor* PageInfoStorageModel::storageAdaptor()
{
	try
	{
		return std::addressof(m_storageAdaptor.value());
	}
	catch (const std::bad_optional_access&)
	{
		return nullptr;
	}
}

QVariant PageInfoStorageModel::data(const QModelIndex& index, int role) const
{
	if (!storageAdaptor() && !index.isValid())
	{
		return QVariant();
	}

	switch (role)
	{
		case Qt::DisplayRole:
		{
			return storageAdaptor()->itemAt(index);
		}

		case Qt::DecorationRole:
		{
			if (storageAdaptor()->itemTypeAt(index) != PageInfo::UrlItemType)
			{
				return QPixmap(":/images/url-icon.png");
			}
		}

		case Qt::BackgroundColorRole:
		{

		}

		case Qt::TextColorRole:
		{

		}
	}

	return QVariant();
}

QVariant PageInfoStorageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (!storageAdaptor() || orientation == Qt::Vertical)
	{
		return QVariant();
	}

	return storageAdaptor()->columnDescription(section);
}

int PageInfoStorageModel::columnCount(const QModelIndex&) const
{
	if (!storageAdaptor())
	{
		return 0;
	}

	return storageAdaptor()->availableColumns().size();
}

int PageInfoStorageModel::rowCount(const QModelIndex& parent) const
{
	if (!storageAdaptor())
	{
		return 0;
	}

	return storageAdaptor()->itemCount();
}

}