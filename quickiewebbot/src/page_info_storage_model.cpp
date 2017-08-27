#include "page_info_storage_model.h"
#include "default_column_resize_policy.h"
#include "storage_adaptor.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

PageInfoStorageModel::PageInfoStorageModel(QObject* parent)
	: ITableModel(parent)
	, m_storageAdaptor(nullptr)
	, m_resizePolicy(std::make_shared<DefaultColumnResizePolicy>())
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
	if (m_storageAdaptor == storageAdaptor)
	{
		return;
	}

	beginResetModel();

	disconnect(m_storageAdaptor, SIGNAL(pageInfoAdded(int)), this, SLOT(onPageInfoAdded(int)));
	VERIFY(connect(storageAdaptor, SIGNAL(pageInfoAdded(int)), this, SLOT(onPageInfoAdded(int))));
	
	m_storageAdaptor = storageAdaptor;

	std::map<int, int> columnsWidth;

	for (int i = 0; i < m_storageAdaptor->availableColumns().size() + 1; ++i)
	{
		if (i == 0)
		{
			// set first column width which shows row number

			columnsWidth[0] = QuickieWebBotHelpers::pointsToPixels(30);
			continue;
		}

		// set other column widths

		columnsWidth[i] = PageInfo::columnPrefferedSize(m_storageAdaptor->availableColumns()[i - 1]);
	}

	std::dynamic_pointer_cast<DefaultColumnResizePolicy>(m_resizePolicy)->setColumnsSize(columnsWidth);

	endResetModel();

	emit internalDataChanged();
}

PageInfo::ItemType PageInfoStorageModel::itemType(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		// just return invalid enum for item number

		return PageInfo::BeginEnumPageInfoItemType;
	}

	const QModelIndex validatedIndex = createIndex(index.row(), index.column() - 1);

	return storageAdaptor()->itemType(validatedIndex);
}

IResizePolicy* PageInfoStorageModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
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

	if (role == Qt::BackgroundColorRole)
	{
		return QColor(Qt::white);
	}

	if (role == ITableModel::SelectionBackgroundColorRole)
	{
		return QColor(97, 160, 50, 200);
	}

	if (role == ITableModel::MarginTop ||
		role == ITableModel::MarginBottom)
	{
		return QuickieWebBotHelpers::pointsToPixels(4);
	}

	if (role == ITableModel::MarginLeft ||
		role == ITableModel::MarginRight)
	{
		return QuickieWebBotHelpers::pointsToPixels(2);
	}

	if (index.column() == 0)
	{
		if (role == Qt::DisplayRole)
		{
			return index.row();
		}
		else
		{
			return QVariant();
		}
	}

	QModelIndex validatedIndex = createIndex(index.row(), index.column() - 1);

	switch (role)
	{
		case Qt::DisplayRole:
		{
			return storageAdaptor()->item(validatedIndex);
		}

		case Qt::DecorationRole:
		{
			static QPixmap urlPixmap;

			if (urlPixmap.isNull())
			{
				QPixmap pixmap{ ":/images/url-icon.png" };
				urlPixmap = pixmap.scaled(QuickieWebBotHelpers::pointsToPixels(13.5), QuickieWebBotHelpers::pointsToPixels(13.5));
			}

			if (storageAdaptor()->itemType(validatedIndex) == PageInfo::UrlItemType)
			{
				return urlPixmap;
			}

			break;
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
		if (section == 0)
		{
			return QString(tr("#"));
		}

		return storageAdaptor()->columnDescription(section - 1);
	}

	return QVariant();
}

int PageInfoStorageModel::columnCount(const QModelIndex&) const
{
	if (!storageAdaptor())
	{
		return 0;
	}

	return storageAdaptor()->availableColumns().size() + 1;
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