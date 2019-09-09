#include "stdafx.h"
#include "page_model.h"
#include "default_column_resize_policy.h"
#include "parsed_page_info_storage_adapter.h"
#include "helpers.h"

namespace SeoSpider
{

PageModel::PageModel(QObject* parent)
	: AbstractTableModel(parent)
	, m_storageAdapter(nullptr)
	, m_resizePolicy(std::make_shared<DefaultColumnResizePolicy>())
{
}

Qt::ItemFlags PageModel::flags(const QModelIndex& index) const
{
	Q_UNUSED(index);

	if (!m_storageAdapter)
	{
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PageModel::setStorageAdapter(IStorageAdapter* storageAdapter) noexcept
{
	DEBUG_ASSERT(storageAdapter->columnCount());

	IStorageAdapter* oldStorageAdapter = m_storageAdapter;

	if (m_storageAdapter == storageAdapter)
	{
		return;
	}

	beginResetModel();

	if (m_storageAdapter)
	{
		disconnect(m_storageAdapter->qobject(), SIGNAL(parsedPageInfoAdded(int)), this, SLOT(onParsedPageInfoAdded(int)));
		disconnect(m_storageAdapter->qobject(), SIGNAL(parsedPageInfoRemoved(int)), this, SLOT(onParsedPageInfoRemoved(int)));
		disconnect(m_storageAdapter->qobject(), SIGNAL(parsedPageInfoReplaced(int)), this, SLOT(onParsedPageInfoReplaced(int)));
		disconnect(m_storageAdapter->qobject(), SIGNAL(repaintIndicesRange(std::pair<int, int>)), this, SLOT(onRepaintIndicesRange(std::pair<int, int>)));
		disconnect(m_storageAdapter->qobject(), SIGNAL(repaintColumn(int)), this, SLOT(onRepaintColumn(int)));
		disconnect(m_storageAdapter->qobject(), SIGNAL(beginClearData()), this, SLOT(onAboutBeginClearingData()));
		disconnect(m_storageAdapter->qobject(), SIGNAL(endClearData()), this, SLOT(onAboutEndClearingData()));
	}

	VERIFY(connect(storageAdapter->qobject(), SIGNAL(parsedPageInfoAdded(int)), this, SLOT(onParsedPageInfoAdded(int))));
	VERIFY(connect(storageAdapter->qobject(), SIGNAL(parsedPageInfoRemoved(int)), this, SLOT(onParsedPageInfoRemoved(int))));
	VERIFY(connect(storageAdapter->qobject(), SIGNAL(parsedPageInfoReplaced(int)), this, SLOT(onParsedPageInfoReplaced(int))));
	VERIFY(connect(storageAdapter->qobject(), SIGNAL(repaintIndicesRange(std::pair<int, int>)), this, SLOT(onRepaintIndicesRange(std::pair<int, int>))));
	VERIFY(connect(storageAdapter->qobject(), SIGNAL(repaintColumn(int)), this, SLOT(onRepaintColumn(int))));
	VERIFY(connect(storageAdapter->qobject(), SIGNAL(beginClearData()), this, SLOT(onAboutBeginClearingData())));
	VERIFY(connect(storageAdapter->qobject(), SIGNAL(endClearData()), this, SLOT(onAboutEndClearingData())));

	m_storageAdapter = storageAdapter;

	std::map<int, int> columnsWidth;

	for (int i = 0; i < m_storageAdapter->columnCount() + 1; ++i)
	{
		if (i == 0)
		{
			// set first column width which shows row number

			columnsWidth[0] = Common::Helpers::pointsToPixels(30);
			continue;
		}

		// set other column widths

		columnsWidth[i] = m_storageAdapter->columnWidth(i - 1);
	}

	std::dynamic_pointer_cast<DefaultColumnResizePolicy>(m_resizePolicy)->setColumnsSize(columnsWidth);

	endResetModel();

	emit internalDataChanged();

	if (oldStorageAdapter)
	{
		oldStorageAdapter->qobject()->deleteLater();
	}
}

IStorageAdapter::ItemType PageModel::itemType(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		// just return invalid enum for item number

		return IStorageAdapter::ItemType::PlainItemType;
	}

	const QModelIndex validatedIndex = createIndex(index.row(), index.column() - 1);

	return storageAdapter()->itemType(validatedIndex);
}

IResizePolicy* PageModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}

const IStorageAdapter* PageModel::storageAdapter() const
{
	return m_storageAdapter;
}

IStorageAdapter* PageModel::storageAdapter()
{
	return m_storageAdapter;
}

QVariant PageModel::data(const QModelIndex& index, int role) const
{
	if (!storageAdapter() || !index.isValid())
	{
		return QVariant();
	}

	if (role == AbstractTableModel::underlyingIndexRole)
	{
		return index;
	}

	if (index.column() == 0)
	{
		if (role == Qt::DisplayRole)
		{
			return index.row() + 1;
		}
		else
		{
			return QVariant();
		}
	}

	const QModelIndex validatedIndex = createIndex(index.row(), index.column() - 1);

	if (role == AbstractTableModel::resourceTypeRole)
	{
		return static_cast<int>(storageAdapter()->resourceType(validatedIndex));
	}

	switch (role)
	{
		case Qt::DisplayRole:
		{
			return storageAdapter()->item(validatedIndex);
		}
	}

	return QVariant();
}

QVariant PageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (!storageAdapter())
	{
		return QVariant();
	}

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == 0)
		{
			return QString(tr("#"));
		}

		return storageAdapter()->columnDescription(section - 1);
	}

	if (orientation == Qt::Horizontal && role == columnEnabledRole)
	{
		if (section == 0)
		{
			return true;
		}

		return storageAdapter()->columnEnabled(section - 1);
	}

	return QVariant();
}

int PageModel::columnCount(const QModelIndex&) const
{
	if (!storageAdapter())
	{
		return 0;
	}

	return storageAdapter()->columnCount() + 1;
}

int PageModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (!storageAdapter())
	{
		return 0;
	}

	return storageAdapter()->itemCount();
}

void PageModel::onParsedPageInfoAdded(int rowIndex)
{
	beginInsertRows(QModelIndex(), rowIndex, rowIndex);
	endInsertRows();
}

void PageModel::onParsedPageInfoRemoved(int rowIndex)
{
	beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
	endRemoveRows();
}

void PageModel::onParsedPageInfoReplaced(int rowIndex)
{
	const QModelIndex startIndexChanged = index(rowIndex, 0);
	const QModelIndex endIndexChanged = index(rowIndex, storageAdapter()->columnCount() - 1);

	emit dataChanged(startIndexChanged, endIndexChanged);
}

void PageModel::onRepaintIndicesRange(std::pair<int, int> indicesRange)
{
	const QModelIndex startIndexChanged = index(indicesRange.first, 0);
	const QModelIndex endIndexChanged = index(indicesRange.second, storageAdapter()->columnCount() - 1);

	emit dataChanged(startIndexChanged, endIndexChanged);
}

void PageModel::onRepaintColumn(int column)
{
	const QModelIndex startIndexChanged = index(0, column);
	const QModelIndex endIndexChanged = index(storageAdapter()->itemCount() - 1, column);

	emit dataChanged(startIndexChanged, endIndexChanged);
}

void PageModel::onAboutBeginClearingData()
{
	beginResetModel();
}

void PageModel::onAboutEndClearingData()
{
	emit internalDataChanged();
	endResetModel();
}

}