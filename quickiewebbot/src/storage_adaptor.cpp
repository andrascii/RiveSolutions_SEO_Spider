#include "storage_adaptor.h"

namespace QuickieWebBot
{

StorageAdaptor::StorageAdaptor(const DataCollection::GuiStorageTypePtr& associatedStorage, QObject* parent)
	: QObject(parent)
	, m_associatedStorage(associatedStorage)
{
}

void StorageAdaptor::setAvailableColumns(QList<PageInfo::ItemType> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<PageInfo::ItemType> StorageAdaptor::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString StorageAdaptor::columnDescription(int columnIndex) const noexcept
{
	assert(columnIndex < m_availableColumns.size());

	return PageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int StorageAdaptor::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant StorageAdaptor::itemAt(const QModelIndex& index) const noexcept
{
	const DataCollection::GuiStorageType& storage = *m_associatedStorage;

	assert(index.row() < storage.size());
	assert(index.column() < m_availableColumns.size());

	return storage[index.row()]->itemValue(m_availableColumns[index.column()]);
}

PageInfo::ItemType StorageAdaptor::itemTypeAt(const QModelIndex& index) const noexcept
{
	assert(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()];
}

}