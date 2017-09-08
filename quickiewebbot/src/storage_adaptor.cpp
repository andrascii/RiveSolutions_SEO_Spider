#include "storage_adaptor.h"

namespace QuickieWebBot
{

StorageAdaptor::StorageAdaptor(const WebCrawler::GuiStorage::GuiStorageTypePtr& associatedStorage,
	WebCrawler::DataCollection::StorageType storageType, QObject* parent)
	: QObject(parent)
	, m_associatedStorage(associatedStorage)
	, m_storageType(storageType)
{
	WebCrawler::GuiStorage* guiStorage = theApp->guiStorage();

	DEBUG_ASSERT(guiStorage);

	QObject::connect(guiStorage, &WebCrawler::GuiStorage::pageRawAdded, this, &StorageAdaptor::onStorageUpdated);
}

void StorageAdaptor::setAvailableColumns(QList<PageRawInfo::ItemType> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<PageRawInfo::ItemType> StorageAdaptor::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString StorageAdaptor::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return PageRawInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int StorageAdaptor::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant StorageAdaptor::item(const QModelIndex& index) const noexcept
{
	const WebCrawler::GuiStorage::GuiStorageType& storage = *m_associatedStorage;

	DEBUG_ASSERT(index.row() < storage.size());
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return PageRawInfo(storage[index.row()]).itemValue(m_availableColumns[index.column()]);
}

PageRawInfo::ItemType StorageAdaptor::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()];
}

#ifdef QT_DEBUG
WebCrawler::PageRaw* StorageAdaptor::pageRaw(const QModelIndex& index) const noexcept
{
	const WebCrawler::GuiStorage::GuiStorageType& storage = *m_associatedStorage;
	return storage[index.row()].get();
}
#endif // DEBUG


void StorageAdaptor::onStorageUpdated(int row, int type)
{
	if (type != m_storageType)
	{
		return;
	}

	Q_EMIT pageInfoAdded(row);
}

}