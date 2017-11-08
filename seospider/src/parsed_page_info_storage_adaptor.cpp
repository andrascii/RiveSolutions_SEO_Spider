#include "parsed_page_info_storage_adaptor.h"
#include "application.h"

namespace SeoSpider
{

ParsedPageInfoStorageAdaptor::ParsedPageInfoStorageAdaptor(
	const CrawlerEngine::SequencedStorage* associatedStorage,
	CrawlerEngine::StorageType storageType, QObject* parent)
	: QObject(parent)
	, m_associatedStorage(associatedStorage)
	, m_storageType(storageType)
{
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();

	DEBUG_ASSERT(sequencedDataCollection);

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded, 
		this, &ParsedPageInfoStorageAdaptor::onStorageUpdated));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &ParsedPageInfoStorageAdaptor::beginClearData));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::endClearData,
		this, &ParsedPageInfoStorageAdaptor::endClearData));
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
	const CrawlerEngine::SequencedStorage& storage = *m_associatedStorage;

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
	const CrawlerEngine::ParsedPage* parsedPage;

	if (itemCount() > index.row())
	{
		parsedPage = (*m_associatedStorage)[index.row()];
	}

	return std::make_shared<ParsedPageInfo>(parsedPage);
}

QObject* ParsedPageInfoStorageAdaptor::qobject() noexcept
{
	return this;
}

#ifdef QT_DEBUG
const CrawlerEngine::ParsedPage* ParsedPageInfoStorageAdaptor::parsedPage(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::SequencedStorage& storage = *m_associatedStorage;
	return storage[index.row()];
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