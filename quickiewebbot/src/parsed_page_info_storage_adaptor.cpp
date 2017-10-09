#include "parsed_page_info_storage_adaptor.h"
#include "application.h"

namespace QuickieWebBot
{

ParsedPageInfoStorageAdaptor::ParsedPageInfoStorageAdaptor(const WebCrawler::SequencedDataCollection::SequencedStorageTypePtr& associatedStorage,
	WebCrawler::StorageType storageType, QObject* parent)
	: QObject(parent)
	, m_associatedStorage(associatedStorage)
	, m_storageType(storageType)
{
	WebCrawler::SequencedDataCollection* guiStorage = theApp->sequencedDataCollection();

	DEBUG_ASSERT(guiStorage);

	VERIFY(connect(guiStorage, &WebCrawler::SequencedDataCollection::parsedPageAdded, 
		this, &ParsedPageInfoStorageAdaptor::onStorageUpdated));
}

void ParsedPageInfoStorageAdaptor::setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<ParsedPageInfo::Column> ParsedPageInfoStorageAdaptor::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString ParsedPageInfoStorageAdaptor::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int ParsedPageInfoStorageAdaptor::columnWidth(int columnNumber) const noexcept
{
	DEBUG_ASSERT(columnNumber < m_availableColumns.size());

	return ParsedPageInfo::columnPrefferedSize(m_availableColumns[columnNumber]);
}

int ParsedPageInfoStorageAdaptor::columnCount() const noexcept
{
	return m_availableColumns.size();
}

int ParsedPageInfoStorageAdaptor::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant ParsedPageInfoStorageAdaptor::item(const QModelIndex& index) const noexcept
{
	const WebCrawler::SequencedDataCollection::SequencedStorageType& storage = *m_associatedStorage;

	DEBUG_ASSERT(index.row() < storage.size());
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return ParsedPageInfo(storage[index.row()]).itemValue(m_availableColumns[index.column()]);
}

ParsedPageInfoStorageAdaptor::ItemType ParsedPageInfoStorageAdaptor::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()] == ParsedPageInfo::Column::UrlColumn ?
		ItemType::UrlItemType :
		ItemType::PlainItemType;
}


ParsedPageInfoPtr ParsedPageInfoStorageAdaptor::parsedPageInfoPtr(const QModelIndex& index) const noexcept
{
	WebCrawler::ParsedPagePtr ptrToPageRawData;

	if (itemCount() > index.row())
	{
		ptrToPageRawData = (*m_associatedStorage)[index.row()];
	}

	return std::make_shared<ParsedPageInfo>(ptrToPageRawData);
}

QObject* ParsedPageInfoStorageAdaptor::qobject() noexcept
{
	return this;
}

#ifdef QT_DEBUG
WebCrawler::ParsedPage* ParsedPageInfoStorageAdaptor::parsedPage(const QModelIndex& index) const noexcept
{
	const WebCrawler::SequencedDataCollection::SequencedStorageType& storage = *m_associatedStorage;
	return storage[index.row()].get();
}
#endif


void ParsedPageInfoStorageAdaptor::onStorageUpdated(int row, int type)
{
	if (type != m_storageType)
	{
		return;
	}

	Q_EMIT parsedPageInfoAdded(row);
}

}