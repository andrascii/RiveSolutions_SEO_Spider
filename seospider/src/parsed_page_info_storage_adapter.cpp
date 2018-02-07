#include "parsed_page_info_storage_adapter.h"
#include "isequenced_storage.h"
#include "application.h"
#include "crawler.h"

namespace SeoSpider
{

ParsedPageInfoStorageAdapter::ParsedPageInfoStorageAdapter(
	CrawlerEngine::SequencedDataCollection* dataCollection,
	CrawlerEngine::ISequencedStorage* associatedStorage,
	CrawlerEngine::StorageType storageType, QObject* parent)
	: QObject(parent)
	, m_dataCollection(dataCollection)
	, m_associatedStorage(associatedStorage)
	, m_storageType(storageType)
{
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();

	DEBUG_ASSERT(sequencedDataCollection);

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded,
		this, &ParsedPageInfoStorageAdapter::onStorageUpdated));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageRemoved,
		this, &ParsedPageInfoStorageAdapter::onPageRemoved));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageReplaced,
		this, &ParsedPageInfoStorageAdapter::onPageReplaced));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::indicesRangeInvalidated,
		this, &ParsedPageInfoStorageAdapter::onRepaintIndicesRange));

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

Menu ParsedPageInfoStorageAdapter::menuFor(const QModelIndex& index) const
{
	Menu menu;

	if (!index.isValid())
	{
		return menu;
	}

	const auto urlColumnIterator = std::find(m_availableColumns.begin(), m_availableColumns.end(), ParsedPageInfo::Column::UrlColumn);
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	if (urlColumnIterator != m_availableColumns.end())
	{
		const QVariant urlItem = ParsedPageInfo(storage[index.row()]).itemValue(*urlColumnIterator);
		DEBUG_ASSERT(urlItem.type() == QVariant::Url);

		const CrawlerEngine::Url url = urlItem.toUrl();

		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenUrlCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<CheckGoogleCacheCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<CheckHTMLWithW3CValidatorCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenInWaybackMachineCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenRobotsTxtFileCommand>()));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<RefreshPageCommand>(m_storageType, index.row())));

		std::shared_ptr<Menu> copySubMenu = std::make_shared<Menu>(tr("Copy..."));
		copySubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<CopyToClipboardAllPagesCommand>(m_associatedStorage)));
		copySubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<CopyToClipboardAllColumnsDataCommand>(m_associatedStorage, m_storageType, index.row())));
		copySubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<CopyToClipboardUrlCommand>(m_associatedStorage, index.row())));
		menu.addItem(copySubMenu);

		std::shared_ptr<Menu> exportSubMenu = std::make_shared<Menu>(tr("Export..."));
		exportSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<ExportUrlInfoToXlsxCommand>(m_associatedStorage, m_availableColumns, index.row())));
		exportSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<ExportUrlOutlinksToXlsxCommand>(m_associatedStorage, index.row())));
		exportSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<ExportUrlInlinksToXlsxCommand>(m_associatedStorage, index.row())));
		menu.addItem(exportSubMenu);

		std::shared_ptr<Menu> goToSubMenu = std::make_shared<Menu>(tr("Go to..."));
		goToSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<GoToLinksOnThisPageCommand>(m_dataCollection, m_associatedStorage, m_storageType, index.row())));
		goToSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<GoToLinksToThisPageCommand>(m_dataCollection, m_associatedStorage, m_storageType, index.row())));
		goToSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<GoToHTTPResponseCommand>(m_dataCollection, m_associatedStorage, m_storageType, index.row())));
		menu.addItem(goToSubMenu);
	}

	const std::optional<QByteArray> ipv4Address = theApp->crawler()->currentCrawledSiteIPv4();

	if (!storage.empty() && ipv4Address.has_value())
	{
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<ShowOtherDomainsOnIpCommand>(ipv4Address.value())));
	}

	return menu;
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

void ParsedPageInfoStorageAdapter::onPageRemoved(int row, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	emit parsedPageInfoRemoved(row);
}

void ParsedPageInfoStorageAdapter::onPageReplaced(int row, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	emit parsedPageInfoReplaced(row);
}

void ParsedPageInfoStorageAdapter::onRepaintIndicesRange(std::pair<int, int> indicesRange, CrawlerEngine::StorageType type) const
{
	if (type != m_storageType)
	{
		return;
	}

	emit repaintIndicesRange(indicesRange);
}

}