#include "parsed_page_info_storage_adapter.h"
#include "isequenced_storage.h"
#include "application.h"
#include "crawler.h"

namespace SeoSpider
{

ParsedPageInfoStorageAdapter::ParsedPageInfoStorageAdapter(
	CrawlerEngine::ISequencedStorage* associatedStorage,
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

void ParsedPageInfoStorageAdapter::setAvailableColumns(QVector<ParsedPageInfo::Column> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QVector<ParsedPageInfo::Column> ParsedPageInfoStorageAdapter::availableColumns() const noexcept
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

	const bool isIndexMappedToUrl = 
		m_availableColumns[index.column()] == ParsedPageInfo::Column::UrlColumn ||
		m_availableColumns[index.column()] == ParsedPageInfo::Column::RedirectedUrlColumn ||
		m_availableColumns[index.column()] == ParsedPageInfo::Column::CanonicalLinkElementColumn;

	return isIndexMappedToUrl ? ItemType::UrlItemType : ItemType::PlainItemType;
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

std::vector<ICommandPointer> ParsedPageInfoStorageAdapter::commandsFor(const QModelIndex& index) const
{
	std::vector<ICommandPointer> commands;

	if (!index.isValid())
	{
		return commands;
	}

	const auto urlColumnIterator = std::find(m_availableColumns.begin(), m_availableColumns.end(), ParsedPageInfo::Column::UrlColumn);
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	if (urlColumnIterator != m_availableColumns.end())
	{
		const QVariant urlItem = ParsedPageInfo(storage[index.row()]).itemValue(*urlColumnIterator);
		DEBUG_ASSERT(urlItem.type() == QVariant::Url);

		const CrawlerEngine::Url url = urlItem.toUrl();

		commands.push_back(std::make_shared<OpenUrlCommand>(url));
		commands.push_back(std::make_shared<CheckGoogleCacheCommand>(url));
		commands.push_back(std::make_shared<CheckHTMLWithW3CValidatorCommand>(url));
		commands.push_back(std::make_shared<OpenInWaybackMachineCommand>(url));
		commands.push_back(std::make_shared<CopyToClipboardAllPagesCommand>(m_associatedStorage));
		commands.push_back(std::make_shared<CopyToClipboardAllColumnsDataCommand>(m_associatedStorage, m_storageType, index.row()));
		commands.push_back(std::make_shared<CopyToClipboardUrlCommand>(m_associatedStorage, index.row()));
		commands.push_back(std::make_shared<ExportUrlInfoToXlsxCommand>(m_associatedStorage, m_availableColumns, index.row()));
		commands.push_back(std::make_shared<ExportUrlOutlinksToXlsxCommand>(m_associatedStorage, index.row()));
		commands.push_back(std::make_shared<ExportUrlInlinksToXlsxCommand>(m_associatedStorage, index.row()));
	}

	const std::optional<QByteArray> ipv4Address = theApp->crawler()->currentCrawledSiteIPv4();

	if (!storage.empty() && ipv4Address.has_value())
	{
		commands.push_back(std::make_shared<ShowOtherDomainsOnIpCommand>(ipv4Address.value()));
	}

	return commands;
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

void ParsedPageInfoStorageAdapter::onStorageUpdated(int row, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	emit parsedPageInfoAdded(row);
}

}