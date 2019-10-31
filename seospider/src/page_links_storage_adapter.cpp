#include "stdafx.h"
#include "page_links_storage_adapter.h"
#include "application.h"

namespace SeoSpider
{

PageLinksStorageAdapter::PageLinksStorageAdapter(ParsedPageInfoPtr associatedParsedPage, PageLinkContext context)
	: m_parsedPageInfo(associatedParsedPage)
	, m_context(context)
{
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &PageLinksStorageAdapter::beginClearData));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &PageLinksStorageAdapter::onBeginClearData));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::endClearData,
		this, &PageLinksStorageAdapter::endClearData));

	if (m_context == PageLinkContext::LinksToThisPage)
	{
		qRegisterMetaType<LinksToThisResourceChanges>("LinksToThisResourceChanges");

		VERIFY(connect(sequencedDataCollection, SIGNAL(parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges)),
			this, SLOT(onParsedPageAdded(LinksToThisResourceChanges))));
	}
}

void PageLinksStorageAdapter::setAvailableColumns(QVector<ParsedPageInfo::PageLinksColumn> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QVector<ParsedPageInfo::PageLinksColumn> PageLinksStorageAdapter::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString PageLinksStorageAdapter::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int PageLinksStorageAdapter::columnWidth(int columnNumber) const noexcept
{
	DEBUG_ASSERT(columnNumber < m_availableColumns.size());

	return ParsedPageInfo::columnPrefferedSize(m_availableColumns[columnNumber]);
}

int PageLinksStorageAdapter::columnCount() const noexcept
{
	return m_availableColumns.size();
}

bool PageLinksStorageAdapter::columnEnabled(int column) const noexcept
{
	Q_UNUSED(column);
	return true;
}

int PageLinksStorageAdapter::itemCount() const noexcept
{
	if (!m_parsedPageInfo)
	{
		return 0;
	}

	return static_cast<int>(m_parsedPageInfo->linksCount(m_context));
}

QVariant PageLinksStorageAdapter::item(const QModelIndex& index) const noexcept
{
	if (!m_parsedPageInfo)
	{
		return QVariant();
	}

	DEBUG_ASSERT(static_cast<size_t>(index.row()) < m_parsedPageInfo->linksCount(m_context));
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_parsedPageInfo->itemValue(m_availableColumns[index.column()], m_context, index.row());
}

PageLinksStorageAdapter::ItemType PageLinksStorageAdapter::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()] == ParsedPageInfo::PageLinksColumn::UrlColumn ?
		ItemType::UrlItemType :
		ItemType::PlainItemType;
}

RowResourceType PageLinksStorageAdapter::resourceType(const QModelIndex& index) const noexcept
{
	if (!m_parsedPageInfo)
	{
		return ResourceNone;
	}

	DEBUG_ASSERT(static_cast<size_t>(index.row()) < m_parsedPageInfo->linksCount(m_context));
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_parsedPageInfo->resourceType(m_context, index.row());
}

QString PageLinksStorageAdapter::canonicalUrl(const QModelIndex& index) const noexcept
{
	Q_UNUSED(index);
	static QString s_empty;
	return s_empty;
}

ParsedPageInfoPtr PageLinksStorageAdapter::parsedPageInfoPtr(const QModelIndex& index) const noexcept
{
	Q_UNUSED(index);

	return m_parsedPageInfo;
}

Menu PageLinksStorageAdapter::menuFor(const QModelIndex& index) const
{
	Menu menu;

	const auto urlColumnIterator = std::find(m_availableColumns.begin(), m_availableColumns.end(), ParsedPageInfo::PageLinksColumn::UrlColumn);

	if (urlColumnIterator != m_availableColumns.end())
	{
		const QVariant urlItem = m_parsedPageInfo->itemValue(*urlColumnIterator, m_context, index.row());
		DEBUG_ASSERT(urlItem.type() == QVariant::Url);

		const Url url = urlItem.toUrl();

		menu.addItem(std::make_shared<CommandMenuItem>(std::make_shared<OpenUrlCommand>(url)));
	}

	return menu;
}

QObject* PageLinksStorageAdapter::qobject() noexcept
{
	return this;
}

void PageLinksStorageAdapter::onBeginClearData()
{
	emit beginClearData();

	m_parsedPageInfo.reset();
}

void PageLinksStorageAdapter::onParsedPageAdded(LinksToThisResourceChanges changes)
{
	if (!m_parsedPageInfo)
	{
		return;
	}

	std::vector<size_t> newRowIndices;

	for (const auto&[parsedPageWeakPointer, fromIndex] : changes.changes)
	{
		if (parsedPageWeakPointer.expired())
		{
			WARNLOG << "pointer expired";
			continue;
		}

		if (parsedPageWeakPointer.lock().get() != m_parsedPageInfo->associatedParsedPage())
		{
			continue;
		}

		newRowIndices.push_back(fromIndex);
	}

	if (newRowIndices.empty())
	{
		return;
	}

	const auto findMinMaxIndex = [](size_t lhs, size_t rhs)
	{
		return lhs < rhs;
	};

	const auto minElementIter = std::min_element(newRowIndices.begin(), newRowIndices.end(), findMinMaxIndex);
	const auto maxElementIter = std::max_element(newRowIndices.begin(), newRowIndices.end(), findMinMaxIndex);

	ASSERT(minElementIter != newRowIndices.end());
	ASSERT(maxElementIter != newRowIndices.end());

	for (size_t i = *minElementIter, maxElement = *maxElementIter; i <= maxElement; ++i)
	{
		emit parsedPageInfoAdded(static_cast<int>(i));
	}
}

}