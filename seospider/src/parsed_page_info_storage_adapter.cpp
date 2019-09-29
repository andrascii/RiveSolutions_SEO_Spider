#include "stdafx.h"
#include "parsed_page_info_storage_adapter.h"
#include "isequenced_storage.h"
#include "application.h"
#include "crawler.h"
#include "model_helpers.h"
#include "helpers.h"

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
	, m_linksToThisPageIndex(-1)
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

void ParsedPageInfoStorageAdapter::setAvailableColumns(const QVector<ParsedPageInfo::Column>& availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

const QVector<ParsedPageInfo::Column>& ParsedPageInfoStorageAdapter::availableColumns() const noexcept
{
	return m_availableColumns;
}

void ParsedPageInfoStorageAdapter::setCurrentColumns(const QVector<ParsedPageInfo::Column>& currentColumns) noexcept
{
	m_currentColumns = currentColumns;
	m_linksToThisPageIndex = m_currentColumns.indexOf(ParsedPageInfo::Column::LinksToThisPageCountColumn);
}

const QVector<ParsedPageInfo::Column>& ParsedPageInfoStorageAdapter::currentColumns() const noexcept
{
	return m_currentColumns;
}

QString ParsedPageInfoStorageAdapter::columnDescription(int columnIndex) const noexcept
{
	ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int ParsedPageInfoStorageAdapter::columnWidth(int columnIndex) const noexcept
{
	ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::columnPrefferedSize(m_availableColumns[columnIndex]);
}

int ParsedPageInfoStorageAdapter::columnCount() const noexcept
{
	return m_availableColumns.size();
}

bool ParsedPageInfoStorageAdapter::columnEnabled(int columnIndex) const noexcept
{
	ASSERT(columnIndex < m_availableColumns.size() && columnIndex >= 0);
	auto it = std::find(m_currentColumns.cbegin(), m_currentColumns.cend(), m_availableColumns[columnIndex]);
	return it != m_currentColumns.cend();
}

int ParsedPageInfoStorageAdapter::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant ParsedPageInfoStorageAdapter::item(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	ASSERT(index.row() < storage.size());
	ASSERT(index.column() < m_availableColumns.size());

	return ParsedPageInfo(storage[index.row()]).itemValue(m_availableColumns[index.column()]);
}

ParsedPageInfoStorageAdapter::ItemType ParsedPageInfoStorageAdapter::itemType(const QModelIndex& index) const noexcept
{
	ASSERT(index.column() < m_availableColumns.size());

	const bool isIndexMappedToUrl =
		m_availableColumns[index.column()] == ParsedPageInfo::Column::UrlColumn ||
		m_availableColumns[index.column()] == ParsedPageInfo::Column::RedirectedUrlColumn ||
		m_availableColumns[index.column()] == ParsedPageInfo::Column::CanonicalLinkElementColumn;

	return isIndexMappedToUrl ? ItemType::UrlItemType : ItemType::PlainItemType;
}

RowResourceType ParsedPageInfoStorageAdapter::resourceType(int row) const noexcept
{
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	ASSERT(row < storage.size());

	return resourceTypeToRowResourceType(storage[row]->resourceType);
}

ParsedPageInfoPtr ParsedPageInfoStorageAdapter::parsedPageInfoPtr(const QModelIndex& index) const noexcept
{
	return Common::make_counted<ParsedPageInfo>(parsedPage(index));
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
		int row = getUnderlyingIndex(index).row();

		const QVariant urlItem = ParsedPageInfo(storage[row]).itemValue(*urlColumnIterator);
		DEBUG_ASSERT(urlItem.type() == QVariant::Url);

		const CrawlerEngine::Url url = urlItem.toUrl();

		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<OpenUrlCommand>(url)));
		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<CheckGoogleCacheCommand>(url)));
		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<CheckHTMLWithW3CValidatorCommand>(url)));
		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<OpenInWaybackMachineCommand>(url)));
		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<OpenRobotsTxtFileCommand>()));
		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<RefreshPageCommand>(m_storageType, row)));


		Common::counted_ptr<Menu> copySubMenu = Common::make_counted<Menu>(tr("Copy..."));
		copySubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<CopyToClipboardAllPagesCommand>(m_associatedStorage)));
		copySubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<CopyToClipboardAllColumnsDataCommand>(m_associatedStorage, m_storageType, row)));
		copySubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<CopyToClipboardUrlCommand>(m_associatedStorage, row)));
		menu.addItem(copySubMenu);

		Common::counted_ptr<Menu> exportSubMenu = Common::make_counted<Menu>(tr("Export..."));
		exportSubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<ExportUrlInfoToXlsxCommand>(m_associatedStorage, m_availableColumns, row)));
		exportSubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<ExportUrlOutlinksToXlsxCommand>(m_associatedStorage, row)));
		exportSubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<ExportUrlInlinksToXlsxCommand>(m_associatedStorage, row)));
		menu.addItem(exportSubMenu);

		Common::counted_ptr<Menu> goToSubMenu = Common::make_counted<Menu>(tr("Go to..."));
		goToSubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<GoToLinksOnThisPageCommand>()));
		goToSubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<GoToLinksToThisPageCommand>()));
		goToSubMenu->addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<GoToHTTPResponseCommand>()));
		menu.addItem(goToSubMenu);
	}

	const std::optional<QByteArray> ipv4Address = theApp->crawler()->currentCrawledSiteIPv4();

	if (!storage.empty() && ipv4Address.has_value())
	{
		menu.addItem(Common::make_counted<CommandMenuItem>(Common::make_counted<ShowOtherDomainsOnIpCommand>(ipv4Address.value())));
	}

	return menu;
}

QObject* ParsedPageInfoStorageAdapter::qobject() noexcept
{
	return this;
}

const CrawlerEngine::ParsedPage* ParsedPageInfoStorageAdapter::parsedPage(const QModelIndex& index) const noexcept
{
	ASSERT(itemCount() > index.row());

	const int row = getUnderlyingIndex(index).row();
	const CrawlerEngine::ParsedPage* parsedPage = (*m_associatedStorage)[row];

	return parsedPage;
}

void ParsedPageInfoStorageAdapter::onStorageUpdated(int row, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	emit parsedPageInfoAdded(row);
	onRepaintIndicesRange(std::pair(row, row), m_storageType);
}

void ParsedPageInfoStorageAdapter::onPageRemoved(int row, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	emit parsedPageInfoRemoved(row);
	if (m_linksToThisPageIndex != -1)
	{
		emit repaintColumn(m_linksToThisPageIndex);
	}
}

void ParsedPageInfoStorageAdapter::onPageReplaced(int row, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	emit parsedPageInfoReplaced(row);
	if (m_linksToThisPageIndex != -1)
	{
		emit repaintColumn(m_linksToThisPageIndex);
	}
}

void ParsedPageInfoStorageAdapter::onRepaintIndicesRange(std::pair<int, int> indicesRange, CrawlerEngine::StorageType type)
{
	if (type != m_storageType)
	{
		return;
	}

	const int currentSize = m_associatedStorage->size();
	if (currentSize < 1)
	{
		return;
	}

	const std::pair<int, int> fixedRange(std::min(currentSize - 1, indicesRange.first), std::min(currentSize - 1, indicesRange.second));

	emit repaintIndicesRange(fixedRange);
	if (m_linksToThisPageIndex != -1)
	{
		emit repaintColumn(m_linksToThisPageIndex);
	}
}

}
