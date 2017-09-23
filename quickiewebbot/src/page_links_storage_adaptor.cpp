#include "page_links_storage_adaptor.h"

namespace QuickieWebBot
{

PageLinksStorageAdaptor::PageLinksStorageAdaptor(PageRawInfoPtr associatedPageRaw, PageLinkType pageLinkType)
	: m_associatedPageRawInfoPtr(associatedPageRaw)
	, m_pageLinkType(pageLinkType)
{
}

void PageLinksStorageAdaptor::setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept
{
	m_availableColumns = availableColumns;
}

QList<ParsedPageInfo::Column> PageLinksStorageAdaptor::availableColumns() const noexcept
{
	return m_availableColumns;
}

QString PageLinksStorageAdaptor::columnDescription(int columnIndex) const noexcept
{
	DEBUG_ASSERT(columnIndex < m_availableColumns.size());

	return ParsedPageInfo::itemTypeDescription(m_availableColumns[columnIndex]);
}

int PageLinksStorageAdaptor::itemCount() const noexcept
{
	if (!m_associatedPageRawInfoPtr)
	{
		return 0;
	}

	PageRawInfoCountAcceptorMethodType countLinksAcceptor = countLinks();

	return static_cast<int>(((*m_associatedPageRawInfoPtr).*countLinksAcceptor)());
}

QVariant PageLinksStorageAdaptor::item(const QModelIndex& index) const noexcept
{
	if (!m_associatedPageRawInfoPtr)
	{
		return QVariant();
	}

	PageRawInfoLinkAcceptorMethodType linksAcceptor = link();
	WebCrawler::ParsedPageWeakPtr ptr = ((*m_associatedPageRawInfoPtr).*linksAcceptor)(index.row());

	DEBUG_ASSERT(index.row() < itemCount());
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return ParsedPageInfo(ptr.lock()).itemValue(m_availableColumns[index.column()]);
}

ParsedPageInfo::Column PageLinksStorageAdaptor::itemType(const QModelIndex& index) const noexcept
{
	DEBUG_ASSERT(index.column() < m_availableColumns.size());

	return m_availableColumns[index.column()];
}

PageRawInfoPtr PageLinksStorageAdaptor::pageRawInfoPtr(const QModelIndex& index) const noexcept
{
	return m_associatedPageRawInfoPtr;
}

QObject* PageLinksStorageAdaptor::qobject() noexcept
{
	return this;
}

PageLinksStorageAdaptor::PageRawInfoCountAcceptorMethodType PageLinksStorageAdaptor::countLinks() const noexcept
{
	PageRawInfoCountAcceptorMethodType func = nullptr;

	switch (m_pageLinkType)
	{
		case PageLinkType::LinksOnThisPageType:
		{
			func = &ParsedPageInfo::countLinksFromThisPage;
			break;
		}
		case PageLinkType::LinksToThisPageType:
		{
			func = &ParsedPageInfo::countLinksToThisPage;
			break;
		}
	}

	return func;
}

PageLinksStorageAdaptor::PageRawInfoLinkAcceptorMethodType PageLinksStorageAdaptor::link() const noexcept
{
	PageRawInfoLinkAcceptorMethodType func = nullptr;

	switch (m_pageLinkType)
	{
		case PageLinkType::LinksOnThisPageType:
		{
			func = &ParsedPageInfo::linkFromThisPage;
			break;
		}
		case PageLinkType::LinksToThisPageType:
		{
			func = &ParsedPageInfo::linkToThisPage;
			break;
		}
	}

	return func;
}

}