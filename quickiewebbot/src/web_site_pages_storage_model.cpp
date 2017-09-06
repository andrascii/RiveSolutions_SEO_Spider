#include "web_site_pages_storage_model.h"
#include "default_column_resize_policy.h"
#include "storage_adaptor.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

WebSitePagesStorageModel::WebSitePagesStorageModel(QObject* parent)
	: AbstractTableModel(parent)
	, m_storageAdaptor(nullptr)
	, m_resizePolicy(std::make_shared<DefaultColumnResizePolicy>())
{
}

Qt::ItemFlags WebSitePagesStorageModel::flags(const QModelIndex& index) const
{
	if (!m_storageAdaptor)
	{
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void WebSitePagesStorageModel::setStorageAdaptor(StorageAdaptor* storageAdaptor) noexcept
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

		columnsWidth[i] = PageRawInfo::columnPrefferedSize(m_storageAdaptor->availableColumns()[i - 1]);
	}

	std::dynamic_pointer_cast<DefaultColumnResizePolicy>(m_resizePolicy)->setColumnsSize(columnsWidth);

	endResetModel();

	emit internalDataChanged();
}

PageRawInfo::ItemType WebSitePagesStorageModel::itemType(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		// just return invalid enum for item number

		return PageRawInfo::BeginEnumPageInfoItemType;
	}

	const QModelIndex validatedIndex = createIndex(index.row(), index.column() - 1);

	return storageAdaptor()->itemType(validatedIndex);
}

IResizePolicy* WebSitePagesStorageModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}

const StorageAdaptor* WebSitePagesStorageModel::storageAdaptor() const
{
	return m_storageAdaptor;
}

StorageAdaptor* WebSitePagesStorageModel::storageAdaptor()
{
	return m_storageAdaptor;
}

QVariant WebSitePagesStorageModel::data(const QModelIndex& index, int role) const
{
	if (!storageAdaptor() && !index.isValid())
	{
		return QVariant();
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
	}

	return QVariant();
}

QVariant WebSitePagesStorageModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int WebSitePagesStorageModel::columnCount(const QModelIndex&) const
{
	if (!storageAdaptor())
	{
		return 0;
	}

	return storageAdaptor()->availableColumns().size() + 1;
}

int WebSitePagesStorageModel::rowCount(const QModelIndex& parent) const
{
	if (!storageAdaptor())
	{
		return 0;
	}

	return storageAdaptor()->itemCount();
}

void WebSitePagesStorageModel::onPageInfoAdded(int rowIndex)
{
	beginInsertRows(QModelIndex(), rowIndex, rowIndex);
	
	endInsertRows();
}

void WebSitePagesStorageModel::onPageInfoItemChanged(int row, int column)
{
	QModelIndex indexItemChanged = index(row, column);

	Q_EMIT dataChanged(indexItemChanged, indexItemChanged);
}

}