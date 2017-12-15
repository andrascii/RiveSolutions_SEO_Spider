#include "parsed_page_info_storage_adapter.h"
#include "application.h"

namespace SeoSpider
{

ParsedPageInfoStorageAdapter::ParsedPageInfoStorageAdapter(
	const CrawlerEngine::ISequencedStorage* associatedStorage,
	CrawlerEngine::StorageType storageType, QObject* parent)
	: QObject(parent)
	, m_associatedStorage(associatedStorage)
	, m_storageType(storageType)
{
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();

	DEBUG_ASSERT(sequencedDataCollection);

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded, 
		this, &ParsedPageInfoStorageAdapter::onStorageUpdated));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &ParsedPageInfoStorageAdapter::beginClearData));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::endClearData,
		this, &ParsedPageInfoStorageAdapter::endClearData));
}

void ParsedPageInfoStorageAdapter::setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<ParsedPageInfo::Column> ParsedPageInfoStorageAdapter::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString ParsedPageInfoStorageAdapter::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int ParsedPageInfoStorageAdapter::columnWidth(int columnNumber) const noexcept
{
	DEBUG_ASSERT(columnNumber < m_availableColumns.size());

	return ParsedPageInfo::columnPrefferedSize(m_availableColumns[columnNumber]);
}

int ParsedPageInfoStorageAdapter::columnCount() const noexcept
{
	return m_availableColumns.size();
}

int ParsedPageInfoStorageAdapter::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant ParsedPageInfoStorageAdapter::item(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	DEBUG_ASSERT(index.row() < storage.size());
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return ParsedPageInfo(storage[index.row()]).itemValue(m_availableColumns[index.column()]);
}

ParsedPageInfoStorageAdapter::ItemType ParsedPageInfoStorageAdapter::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()] == ParsedPageInfo::Column::UrlColumn ?
		ItemType::UrlItemType :
		ItemType::PlainItemType;
}


ParsedPageInfoPtr ParsedPageInfoStorageAdapter::parsedPageInfoPtr(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ParsedPage* parsedPage;

	if (itemCount() > index.row())
	{
		parsedPage = (*m_associatedStorage)[index.row()];
	}

	return std::make_shared<ParsedPageInfo>(parsedPage);
}

QObject* ParsedPageInfoStorageAdapter::qobject() noexcept
{
	return this;
}

#ifdef QT_DEBUG
const CrawlerEngine::ParsedPage* ParsedPageInfoStorageAdapter::parsedPage(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;
	return storage[index.row()];
}
#endif


void ParsedPageInfoStorageAdapter::onStorageUpdated(int row, int type)
{
	if (type != m_storageType)
	{
		return;
	}

	Q_EMIT parsedPageInfoAdded(row);
}

}