#include "page_info_storage_model.h"
#include "grid_view_resize_policy.h"
#include "storage_adaptor.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

PageInfoStorageModel::PageInfoStorageModel(QObject* parent)
	: IGridModel(parent)
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
	VERIFY(connect(storageAdaptor, SIGNAL(pageInfoAdded(int)), this, SLOT(onPageInfoAdded(int))));
	
	m_storageAdaptor = storageAdaptor;
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
			static QPixmap urlPixmap(":/images/url-icon.png");

			if (storageAdaptor()->itemTypeAt(index) == PageInfo::UrlItemType)
			{
				return urlPixmap;
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

		case IGridModel::SelectionBackgroundColorRole:
		{
			return QColor(97, 160, 50, 200);
		}

		case IGridModel::MarginTop:
		{
			return QuickieWebBotHelpers::pointsToPixels(2);
		}

		case IGridModel::MarginBottom:
		{
			return QuickieWebBotHelpers::pointsToPixels(2);
		}

		case IGridModel::MarginLeft:
		{
			return QuickieWebBotHelpers::pointsToPixels(4);
		}

		case IGridModel::MarginRight:
		{
			return QuickieWebBotHelpers::pointsToPixels(4);
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
	if (!storageAdaptor())
	{
		return QVariant();
	}

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
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

void PageInfoStorageModel::onPageInfoAdded(int rowIndex)
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