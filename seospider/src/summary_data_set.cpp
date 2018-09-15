#include "summary_data_set.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"
#include "application.h"
#include "preferences.h"

namespace SeoSpider
{

SummaryDataSet::SummaryDataSet(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
{
}

int SummaryDataSet::columnCount() const
{
	return s_summaryColumnCount;
}

int SummaryDataSet::rowCount() const
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

const CrawlerEngine::SequencedDataCollection* SummaryDataSet::sequencedDataCollection() const
{
	return m_sequencedDataCollection;
}

void SummaryDataSet::addGroup(AuditGroup group)
{
	DataCollectionGroupsFactory dcGroupsFactory;
	addGroup(dcGroupsFactory.create(group));
}

void SummaryDataSet::addGroup(DCStorageGroupDescriptionPtr groupPointer)
{
	if (groupPointer->auditGroup == AuditGroup::YandexMetricaCounters)
	{
		VERIFY(connect(theApp->preferences(), &Preferences::searchYandexMetricaCountersChanged,
			this, &SummaryDataSet::searchYandexMetricaCountersChanged));
	}

	m_allGroups.push_back(groupPointer);

	int modelRowIndex = rowCount();
	m_groupRows[modelRowIndex++] = m_allGroups.last();

	for (auto itemStart = std::begin(m_allGroups.last()->descriptions);
		itemStart != std::end(m_allGroups.last()->descriptions);
		++itemStart, ++modelRowIndex)
	{
		m_itemRows[modelRowIndex] = &(*itemStart);
	}

	emit dataSetChanged();
}

void SummaryDataSet::removeGroup(AuditGroup auditGroup)
{
	// TODO: refactor this bullshit

	const auto iter = std::find_if(m_allGroups.begin(), m_allGroups.end(),
		[auditGroup](const DCStorageGroupDescriptionPtr& p) { return p->auditGroup == auditGroup; });

	if (iter == m_allGroups.end())
	{
		return;
	}

	DCStorageGroupDescriptionPtr groupPtr = *iter;
	m_allGroups.erase(iter);

	std::vector<decltype(m_itemRows)::iterator> iteratorsToDelete;

	for(auto it = m_itemRows.begin(); it != m_itemRows.end(); ++it)
	{
		const auto checkStorageType = [&](const DCStorageDescription& description)
		{
			return description.storageType == it.value()->storageType;
		};

		const auto searchingDescription = std::find_if(
			groupPtr->descriptions.begin(),
			groupPtr->descriptions.end(),
			checkStorageType);

		if (searchingDescription != groupPtr->descriptions.end())
		{
			iteratorsToDelete.push_back(it);
		}
	}

	for (int i = 0; i < iteratorsToDelete.size(); ++i)
	{
		m_itemRows.erase(iteratorsToDelete[i]);
	}

	for (auto it = m_groupRows.begin(); it != m_groupRows.end(); ++it)
	{
		if (it.value() == groupPtr)
		{
			m_groupRows.erase(it);
			break;
		}
	}

	emit dataSetChanged();
}

bool SummaryDataSet::isHeaderRow(int row) const
{
	return m_groupRows.find(row) != m_groupRows.end();
}

QSize SummaryDataSet::span(const QModelIndex& index) const
{
	return m_groupRows.find(index.row()) != m_groupRows.end() ? QSize(2, 1) : QSize(1, 1);
}

QVariant SummaryDataSet::item(const QModelIndex& index) const
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

StorageAdapterType SummaryDataSet::itemCategory(const QModelIndex& index) const
{
	if (isHeaderRow(index.row()) || !index.isValid())
	{
		return StorageAdapterType::StorageAdapterTypeNone;
	}

	const DCStorageDescription* description = storageDescriptionByRow(index.row());
	DEBUG_ASSERT(description != nullptr);

	if (description != nullptr)
	{
		return !description->customDataFeed.isEmpty()
			? StorageAdapterType::StorageAdapterTypeCustomDataFeed
			: static_cast<StorageAdapterType>(description->storageType);
	}

	return StorageAdapterType::StorageAdapterTypeNone;
}

QString SummaryDataSet::customDataFeed(const QModelIndex& index) const
{
	if (isHeaderRow(index.row()) || !index.isValid())
	{
		return QString();
	}

	const DCStorageDescription* description = storageDescriptionByRow(index.row());
	DEBUG_ASSERT(description != nullptr);

	if (description == nullptr || description->customDataFeed.isEmpty())
	{
		return QString();
	}

	return description->customDataFeed;
}

const DCStorageDescription* SummaryDataSet::storageDescriptionByRow(int row) const
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

const DCStorageGroupDescription* SummaryDataSet::storageGroupDescriptionByRow(int row) const
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

const DCStorageDescription* SummaryDataSet::storageDescription(CrawlerEngine::StorageType type) const
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

const DCStorageGroupDescription* SummaryDataSet::storageGroupDescription(AuditGroup group) const
{
	foreach(const DCStorageGroupDescriptionPtr& dcStorageGroupDescription, m_groupRows)
	{
		if (dcStorageGroupDescription->auditGroup == group)
		{
			return dcStorageGroupDescription.get();
		}
	}

	return nullptr;
}

void SummaryDataSet::sortItems(int storageRow, CrawlerEngine::StorageType)
{
	// we can sort only one group
	DEBUG_ASSERT(isSortable());

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

void SummaryDataSet::searchYandexMetricaCountersChanged(bool value)
{
	if (!value)
	{
		removeGroup(AuditGroup::YandexMetricaCounters);
	}
	else
	{
		addGroup(AuditGroup::YandexMetricaCounters);
	}
}

bool SummaryDataSet::isSortable() const
{
	return m_allGroups.size() == 1;
}

bool SummaryDataSet::isSortingEnabled() const
{
	return static_cast<bool>(m_sortPredicate);
}

}