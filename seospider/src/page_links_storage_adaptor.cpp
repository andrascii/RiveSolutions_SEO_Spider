#include "page_links_storage_adaptor.h"

namespace SeoSpider
{

PageLinksStorageAdaptor::PageLinksStorageAdaptor(ParsedPageInfoPtr associatedPageRaw, PageLinkContext context)
	: m_parsedPage(associatedPageRaw)
	, m_context(context)
{
}

void PageLinksStorageAdaptor::setAvailableColumns(QList<ParsedPageInfo::PageLinksColumn> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<ParsedPageInfo::PageLinksColumn> PageLinksStorageAdaptor::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString PageLinksStorageAdaptor::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int PageLinksStorageAdaptor::columnWidth(int columnNumber) const noexcept
{
	DEBUG_ASSERT(columnNumber < m_availableColumns.size());

	return ParsedPageInfo::columnPrefferedSize(m_availableColumns[columnNumber]);
}

int PageLinksStorageAdaptor::columnCount() const noexcept
{
	return m_availableColumns.size();
}

int PageLinksStorageAdaptor::itemCount() const noexcept
{
	if (!m_parsedPage)
	{
		return 0;
	}

	return static_cast<int>(m_parsedPage->itemCount(m_context));
}

QVariant PageLinksStorageAdaptor::item(const QModelIndex& index) const noexcept
{
	if (!m_parsedPage)
	{
		return QVariant();
	}

	DEBUG_ASSERT(index.row() < m_parsedPage->itemCount(m_context));
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_parsedPage->itemValue(m_availableColumns[index.column()], m_context, index.row());
}

PageLinksStorageAdaptor::ItemType PageLinksStorageAdaptor::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()] == ParsedPageInfo::PageLinksColumn::UrlColumn ?
		ItemType::UrlItemType :
		ItemType::PlainItemType;
}

ParsedPageInfoPtr PageLinksStorageAdaptor::parsedPageInfoPtr(const QModelIndex& index) const noexcept
{
	return m_parsedPage;
}

QObject* PageLinksStorageAdaptor::qobject() noexcept
{
	return this;
}

}