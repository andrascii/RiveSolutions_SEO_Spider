#include "flexible_links_storage_adapter.h"

namespace SeoSpider
{

FlexibleLinksStorageAdapter::FlexibleLinksStorageAdapter(
	CrawlerEngine::SequencedDataCollection* dataCollection,
	CrawlerEngine::ISequencedStorage* associatedStorage,
	CrawlerEngine::StorageType storageType,
	IFlexibleColumns* flexibleColumns,
	QObject* parent)
	: ParsedPageInfoStorageAdapter(dataCollection, associatedStorage, storageType, parent)
	, m_flexibleColumns(flexibleColumns)
{
	ASSERT(m_flexibleColumns);
	VERIFY(connect(m_flexibleColumns->qobject(), SIGNAL(columnCountChanged(int)), this, SIGNAL(customColumnCountChanged())));
}

QVariant FlexibleLinksStorageAdapter::item(const QModelIndex& index) const noexcept
{
	ASSERT(index.row() < itemCount());
	ASSERT(index.column() < ParsedPageInfoStorageAdapter::columnCount() + m_flexibleColumns->columnCount());

	if (index.column() >= ParsedPageInfoStorageAdapter::columnCount())
	{
		return m_flexibleColumns->data(parsedPage(index), index.column() - ParsedPageInfoStorageAdapter::columnCount());
	}

	return ParsedPageInfoStorageAdapter::item(index);
}

IStorageAdapter::ItemType FlexibleLinksStorageAdapter::itemType(const QModelIndex& index) const noexcept
{
	ASSERT(index.column() < ParsedPageInfoStorageAdapter::columnCount() + m_flexibleColumns->columnCount());

	if (index.column() >= ParsedPageInfoStorageAdapter::columnCount())
	{
		return ItemType::PlainItemType;
	}

	return ParsedPageInfoStorageAdapter::itemType(index);
}

QString FlexibleLinksStorageAdapter::columnDescription(int columnIndex) const noexcept
{
	if (columnIndex < ParsedPageInfoStorageAdapter::columnCount())
	{
		return ParsedPageInfoStorageAdapter::columnDescription(columnIndex);
	}

	return m_flexibleColumns->columnDescription(columnIndex - ParsedPageInfoStorageAdapter::columnCount());
}

int FlexibleLinksStorageAdapter::columnWidth(int columnIndex) const noexcept
{
	if (columnIndex < ParsedPageInfoStorageAdapter::columnCount())
	{
		return ParsedPageInfoStorageAdapter::columnWidth(columnIndex);
	}

	return m_flexibleColumns->columnWidth(columnIndex - ParsedPageInfoStorageAdapter::columnCount());
}

int FlexibleLinksStorageAdapter::columnCount() const noexcept
{
	return ParsedPageInfoStorageAdapter::columnCount() + m_flexibleColumns->columnCount();
}

bool FlexibleLinksStorageAdapter::columnEnabled(int columnIndex) const noexcept
{
	ASSERT(columnIndex < ParsedPageInfoStorageAdapter::columnCount() + m_flexibleColumns->columnCount());

	if (ParsedPageInfoStorageAdapter::columnCount() <= columnIndex)
	{
		return true;
	}

	return ParsedPageInfoStorageAdapter::columnEnabled(columnIndex);
}

}