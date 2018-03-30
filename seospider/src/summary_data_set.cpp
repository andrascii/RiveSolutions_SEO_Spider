#include "summary_data_set.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"

namespace SeoSpider
{

SummaryDataSet::SummaryDataSet(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
{
}

SummaryDataSet::SummaryDataSet(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection, QList<AuditGroup> auditGroups)
	: m_sequencedDataCollection(sequencedDataCollection)
{
	foreach(AuditGroup group, auditGroups)
	{
		addGroup(group);
	}
}

int SummaryDataSet::columnCount() const noexcept
{
	return s_summaryColumnCount;
}

int SummaryDataSet::rowCount() const noexcept
{
	return m_groupRows.size() + m_itemRows.size();
}

void SummaryDataSet::addSortingPredicate(std::function<bool(DCStorageDescription*, DCStorageDescription*)>&& predicate)
{
	m_sortPredicate = std::move(predicate);

	if (m_sortPredicate)
	{
		VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded, this, &SummaryDataSet::sortItems));
	}
	else
	{
		disconnect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded, this, &SummaryDataSet::sortItems);
	}
}

const CrawlerEngine::SequencedDataCollection* SummaryDataSet::sequencedDataCollection() const noexcept
{
	return m_sequencedDataCollection;
}

void SummaryDataSet::addGroup(AuditGroup group) noexcept
{
	DataCollectionGroupsFactory dcGroupsFactory;
	m_allGroups.push_back(dcGroupsFactory.create(group));

	int modelRowIndex = rowCount();
	m_groupRows[modelRowIndex++] = m_allGroups.last();

	for (auto itemStart = std::begin(m_allGroups.last()->descriptions);
		itemStart != std::end(m_allGroups.last()->descriptions);
		++itemStart, ++modelRowIndex)
	{
		m_itemRows[modelRowIndex] = &(*itemStart);
	}
}

bool SummaryDataSet::isHeaderRow(int row) const noexcept
{
	return m_groupRows.find(row) != m_groupRows.end();
}

QSize SummaryDataSet::span(const QModelIndex& index) const noexcept
{
	return m_groupRows.find(index.row()) != m_groupRows.end() ? QSize(2, 1) : QSize(1, 1);
}

QVariant SummaryDataSet::item(const QModelIndex& index) const noexcept
{
	if (isHeaderRow(index.row()))
	{
		return m_groupRows.find(index.row()).value()->name;
	}

	auto itemIterator = m_itemRows.find(index.row());

	ASSERT(itemIterator != m_itemRows.end());

	if (index.column() == 0)
	{
		return itemIterator.value()->storageTypeDescriptionName;
	}

	return m_sequencedDataCollection->storage(itemIterator.value()->storageType)->size();
}

StorageAdapterType SummaryDataSet::itemCategory(const QModelIndex& index) const noexcept
{
	if (isHeaderRow(index.row()) || !index.isValid())
	{
		return StorageAdapterType::StorageAdapterTypeNone;
	}

	return StorageAdapterType{ m_itemRows[index.row()]->storageType };
}

const DCStorageDescription* SummaryDataSet::storageDescriptionByRow(int row) const noexcept
{
	for (auto beg = m_itemRows.begin(); beg != m_itemRows.end(); ++beg)
	{
		if (beg.key() == row)
		{
			return beg.value();
		}
	}

	return nullptr;
}

const DCStorageGroupDescription* SummaryDataSet::storageGroupDescriptionByRow(int row) const noexcept
{
	for (auto beg = m_groupRows.begin(); beg != m_groupRows.end(); ++beg)
	{
		if (beg.key() == row)
		{
			return beg.value().get();
		}
	}

	return nullptr;
}

const DCStorageDescription* SummaryDataSet::storageDescription(CrawlerEngine::StorageType type) const noexcept
{
	foreach(DCStorageDescription* dcStorageDescription, m_itemRows)
	{
		if (dcStorageDescription->storageType == type)
		{
			return dcStorageDescription;
		}
	}

	return nullptr;
}

const DCStorageGroupDescription* SummaryDataSet::storageGroupDescription(AuditGroup group) const noexcept
{
	foreach(const DCStorageGroupDescriptionPtr& dcStorageGroupDescription, m_groupRows)
	{
		if (dcStorageGroupDescription->group == group)
		{
			return dcStorageGroupDescription.get();
		}
	}

	return nullptr;
}

void SummaryDataSet::sortItems(int storageRow, CrawlerEngine::StorageType)
{
	// we can sort only one group
	DEBUG_ASSERT(m_allGroups.size() == 1);

	if (!storageRow)
	{
		emit sortingStarted();

		QVector<DCStorageDescription*> itemRows;

		foreach(auto row, m_itemRows)
		{
			itemRows.append(row);
		}

		qSort(itemRows.begin(), itemRows.end(), m_sortPredicate);

		m_itemRows.clear();

		for (int modelRowsCount = 1; modelRowsCount <= itemRows.size(); ++modelRowsCount)
		{
			m_itemRows[modelRowsCount] = itemRows.at(modelRowsCount - 1);
		}

		emit sortingEnded();
	}
}

}