#include "page_raw_info_storage_adaptor.h"

namespace QuickieWebBot
{

PageRawInfoStorageAdaptor::PageRawInfoStorageAdaptor(const WebCrawler::GuiStorage::GuiStorageTypePtr& associatedStorage,
	WebCrawler::DataCollection::StorageType storageType, QObject* parent)
	: QObject(parent)
	, m_associatedStorage(associatedStorage)
	, m_storageType(storageType)
{
	WebCrawler::GuiStorage* guiStorage = theApp->guiStorage();

	DEBUG_ASSERT(guiStorage);

	QObject::connect(guiStorage, &WebCrawler::GuiStorage::pageRawAdded, this, &PageRawInfoStorageAdaptor::onStorageUpdated);
}

void PageRawInfoStorageAdaptor::setAvailableColumns(QList<PageRawInfo::Column> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<PageRawInfo::Column> PageRawInfoStorageAdaptor::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString PageRawInfoStorageAdaptor::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return PageRawInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int PageRawInfoStorageAdaptor::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant PageRawInfoStorageAdaptor::item(const QModelIndex& index) const noexcept
{
	const WebCrawler::GuiStorage::GuiStorageType& storage = *m_associatedStorage;

	DEBUG_ASSERT(index.row() < storage.size());
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return PageRawInfo(storage[index.row()]).itemValue(m_availableColumns[index.column()]);
}

PageRawInfo::Column PageRawInfoStorageAdaptor::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()];
}


QObject* PageRawInfoStorageAdaptor::qobject() noexcept
{
	return this;
}

#ifdef QT_DEBUG
WebCrawler::PageRaw* PageRawInfoStorageAdaptor::pageRaw(const QModelIndex& index) const noexcept
{
	const WebCrawler::GuiStorage::GuiStorageType& storage = *m_associatedStorage;
	return storage[index.row()].get();
}
#endif // DEBUG


void PageRawInfoStorageAdaptor::onStorageUpdated(int row, int type)
{
	if (type != m_storageType)
	{
		return;
	}

	Q_EMIT pageRawInfoAdded(row);
}

}