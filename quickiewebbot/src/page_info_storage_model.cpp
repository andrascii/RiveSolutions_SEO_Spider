#include "page_info_storage_model.h"
#include "grid_view_resize_strategy.h"
#include "storage_adaptor.h"

namespace QuickieWebBot
{

PageInfoStorageModel::PageInfoStorageModel(QObject* parent)
	: IGridModel(parent)
	, m_resizeStrategy(std::make_unique<GridViewResizeStrategy>())
	, m_storageAdaptor(nullptr)
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

void PageInfoStorageModel::setStorageAdaptor(StorageAdaptor* storageAdaptor) noexcept
{
	VERIFY(connect(storageAdaptor, SIGNAL(pageInfoDataAdded(int)), this, SLOT(onPageInfoDataAdded(int))));
	
	m_storageAdaptor = storageAdaptor;
}

IGridViewResizeStrategy* PageInfoStorageModel::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

const StorageAdaptor* PageInfoStorageModel::storageAdaptor() const
{
	return m_storageAdaptor;
}

StorageAdaptor* PageInfoStorageModel::storageAdaptor()
{
	return m_storageAdaptor;
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
			return QColor(Qt::white);
		}

		case Qt::TextColorRole:
		{
			if (storageAdaptor()->itemTypeAt(index) == PageInfo::UrlItemType)
			{
				return QColor("#343B49");
			}

			return QColor(Qt::black);
		}

		case Qt::FontRole:
		{
			QFont font;

			if (storageAdaptor()->itemTypeAt(index) == PageInfo::UrlItemType)
			{
				font.setBold(true);
				return font;
			}

			return font;
		}

		case IGridModel::WhatsThisRole:
		{
			return storageAdaptor()->itemTypeAt(index);
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

	if (role == Qt::DisplayRole)
	{
		return storageAdaptor()->columnDescription(section);
	}

	return QVariant();
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

void PageInfoStorageModel::onPageInfoDataAdded(int rowIndex)
{
	beginInsertRows(QModelIndex(), rowIndex, rowIndex);
	
	endInsertRows();
}

void PageInfoStorageModel::onPageInfoItemChanged(int row, int column)
{
	QModelIndex indexItemChanged = index(row, column);

	Q_EMIT dataChanged(indexItemChanged, indexItemChanged);
}

}