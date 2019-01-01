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

void ParsedPageInfoStorageAdapter::setCurrentColumns(QVector<ParsedPageInfo::Column> currentColumns) noexcept
{
	m_currentColumns = currentColumns;
}

QVector<ParsedPageInfo::Column> ParsedPageInfoStorageAdapter::currentColumns() const noexcept
{
	return m_currentColumns;
}

void ParsedPageInfoStorageAdapter::setCustomDataFeed(const QString& customDataFeed) noexcept
{
	if (customDataFeed.isEmpty())
	{
		return;
	}

	m_dataFeed = customDataFeed;
	m_customColumns = theApp->crawler()->customDataFeedByName(customDataFeed)->columns().toVector();
}

QString ParsedPageInfoStorageAdapter::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size() + m_customColumns.size());

	if (columnIndex >= m_availableColumns.size())
	{
		return m_customColumns[columnIndex - m_availableColumns.size()];
	}

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int ParsedPageInfoStorageAdapter::columnWidth(int columnNumber) const noexcept
{
	DEBUG_ASSERT(columnNumber < m_availableColumns.size() + m_customColumns.size());

	if (columnNumber >= m_availableColumns.size())
	{
		return Common::Helpers::pointsToPixels(150);
	}

	return ParsedPageInfo::columnPrefferedSize(m_availableColumns[columnNumber]);
}

int ParsedPageInfoStorageAdapter::columnCount() const noexcept
{
	return m_availableColumns.size() + m_customColumns.size();
}

bool ParsedPageInfoStorageAdapter::columnEnabled(int column) const noexcept
{
	DEBUG_ASSERT(column < m_availableColumns.size() && column >= 0);
	auto it = std::find(m_currentColumns.cbegin(), m_currentColumns.cend(), m_availableColumns[column]);
	return it != m_currentColumns.cend();
}

int ParsedPageInfoStorageAdapter::itemCount() const noexcept
{
	return m_associatedStorage->size();
}

QVariant ParsedPageInfoStorageAdapter::item(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	DEBUG_ASSERT(index.row() < storage.size());
	DEBUG_ASSERT(index.column() < m_availableColumns.size() + m_customColumns.size());

	if (index.column() >= m_availableColumns.size())
	{
		const CrawlerEngine::ICustomDataFeed* dataFeed = theApp->crawler()->customDataFeedByName(m_dataFeed);
		const QMap<int, QString>& dataFeedData = storage[index.row()]->dataFeedsData[dataFeed->dataFeedId()];
		return dataFeedData[index.column() - m_availableColumns.size()];
	}

	return ParsedPageInfo(storage[index.row()]).itemValue(m_availableColumns[index.column()]);
}

ParsedPageInfoStorageAdapter::ItemType ParsedPageInfoStorageAdapter::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size() + m_customColumns.size());

	if (index.column() >= m_availableColumns.size())
	{
		return ItemType::PlainItemType;
	}

	const bool isIndexMappedToUrl =
		m_availableColumns[index.column()] == ParsedPageInfo::Column::UrlColumn ||
		m_availableColumns[index.column()] == ParsedPageInfo::Column::RedirectedUrlColumn ||
		m_availableColumns[index.column()] == ParsedPageInfo::Column::CanonicalLinkElementColumn;

	return isIndexMappedToUrl ? ItemType::UrlItemType : ItemType::PlainItemType;
}

RowResourceType ParsedPageInfoStorageAdapter::resourceType(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ISequencedStorage& storage = *m_associatedStorage;

	DEBUG_ASSERT(index.row() < storage.size());
	DEBUG_ASSERT(index.column() < m_availableColumns.size() + m_customColumns.size());

	return resourceTypeToRowResourceType(storage[index.row()]->resourceType);
}

ParsedPageInfoPtr ParsedPageInfoStorageAdapter::parsedPageInfoPtr(const QModelIndex& index) const noexcept
{
	const CrawlerEngine::ParsedPage* parsedPage;
	const int row = getUnderlyingIndex(index).row();

	if (itemCount() > index.row())
	{
		parsedPage = (*m_associatedStorage)[row];
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
		int row = getUnderlyingIndex(index).row();

		const QVariant urlItem = ParsedPageInfo(storage[row]).itemValue(*urlColumnIterator);
		DEBUG_ASSERT(urlItem.type() == QVariant::Url);

		const CrawlerEngine::Url url = urlItem.toUrl();

		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenUrlCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<CheckGoogleCacheCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<CheckHTMLWithW3CValidatorCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenInWaybackMachineCommand>(url)));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenRobotsTxtFileCommand>()));
		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<RefreshPageCommand>(m_storageType, row)));


		std::shared_ptr<Menu> copySubMenu = std::make_shared<Menu>(tr("Copy..."));
		copySubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<CopyToClipboardAllPagesCommand>(m_associatedStorage)));
		copySubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<CopyToClipboardAllColumnsDataCommand>(m_associatedStorage, m_storageType, row)));
		copySubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<CopyToClipboardUrlCommand>(m_associatedStorage, row)));
		menu.addItem(copySubMenu);

		std::shared_ptr<Menu> exportSubMenu = std::make_shared<Menu>(tr("Export..."));
		exportSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<ExportUrlInfoToXlsxCommand>(m_associatedStorage, m_availableColumns, row)));
		exportSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<ExportUrlOutlinksToXlsxCommand>(m_associatedStorage, row)));
		exportSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<ExportUrlInlinksToXlsxCommand>(m_associatedStorage, row)));
		menu.addItem(exportSubMenu);

		std::shared_ptr<Menu> goToSubMenu = std::make_shared<Menu>(tr("Go to..."));
		goToSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<GoToLinksOnThisPageCommand>()));
		goToSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<GoToLinksToThisPageCommand>()));
		goToSubMenu->addItem(std::make_shared<CommandMenuItem>(std::make_shared<GoToHTTPResponseCommand>()));
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
	onRepaintIndicesRange(std::pair(row, row), m_storageType);
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
}

}
