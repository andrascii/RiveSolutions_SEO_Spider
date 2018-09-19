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

void SummaryDataSet::addSortingPredicate(std::function<bool(const DCStorageDescription*, const DCStorageDescription*)>&& predicate)
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

void SummaryDataSet::appendGroup(AuditGroup auditGroup)
{
	DataCollectionGroupsFactory dcGroupsFactory;
	appendGroup(dcGroupsFactory.create(auditGroup));
}

void SummaryDataSet::appendGroup(DCStorageGroupDescriptionPtr groupPointer)
{
	if (!groupPointer)
	{
		ERRLOG << "Some group did not created by the factory";
		return;
	}

	const auto groupIter = std::find_if(m_allGroups.begin(), m_allGroups.end(),
		[&](const DCStorageGroupDescriptionPtr& group) { return group->auditGroup() == groupPointer->auditGroup(); });

	if (groupIter != m_allGroups.end())
	{
		INFOLOG << "Group" << (int)groupPointer->auditGroup() << "already exists and it will be skipped";
		return;
	}

	m_allGroups.push_back(groupPointer);

	int modelRowIndex = rowCount();
	m_groupRows[modelRowIndex++] = groupPointer;

	for (auto itemStart = std::begin(groupPointer->descriptions());
		itemStart != std::end(groupPointer->descriptions());
		++itemStart, ++modelRowIndex)
	{
		m_itemRows[modelRowIndex] = &(*itemStart);
	}

	VERIFY(connect(groupPointer.get(), &DCStorageGroupDescription::changed,
		this, &SummaryDataSet::reconfigureFilters));

	emit dataSetChanged();
}

void SummaryDataSet::prependGroup(AuditGroup auditGroup)
{
	QVector<AuditGroup> auditGroupsSequentialChainData = auditGroupsSequentialChain();

	removeFilters();

	auditGroupsSequentialChainData.prepend(auditGroup);

	std::for_each(auditGroupsSequentialChainData.begin(), auditGroupsSequentialChainData.end(),
		[this](AuditGroup auditGroup) { appendGroup(auditGroup); });
}

void SummaryDataSet::removeGroup(AuditGroup auditGroup)
{
	QVector<AuditGroup> auditGroupsSequentialChainData = auditGroupsSequentialChain();

	removeFilters();

	auditGroupsSequentialChainData.removeOne(auditGroup);

	std::for_each(auditGroupsSequentialChainData.begin(), auditGroupsSequentialChainData.end(),
		[this](AuditGroup auditGroup) { appendGroup(auditGroup); });
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
		return m_groupRows.find(index.row()).value()->name();
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
		return !description->customDataFeed.isEmpty() ?
			StorageAdapterType::StorageAdapterTypeCustomDataFeed :
			static_cast<StorageAdapterType>(description->storageType);
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
	foreach(const DCStorageDescription* dcStorageDescription, m_itemRows)
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
		if (dcStorageGroupDescription->auditGroup() == group)
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

		QVector<const DCStorageDescription*> itemRows;

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

void SummaryDataSet::reconfigureFilters()
{
	QVector<AuditGroup> auditGroupsSequentialChainData = auditGroupsSequentialChain();

	removeFilters();

	std::for_each(auditGroupsSequentialChainData.begin(), auditGroupsSequentialChainData.end(),
		[this](AuditGroup auditGroup) { appendGroup(auditGroup); });
}

QVector<AuditGroup> SummaryDataSet::auditGroupsSequentialChain() const
{
	QVector<AuditGroup> auditGroupsSequentialChainData;

	foreach(const DCStorageGroupDescriptionPtr& groupPointer, m_groupRows)
	{
		auditGroupsSequentialChainData.push_back(groupPointer->auditGroup());
	}

	return auditGroupsSequentialChainData;
}

void SummaryDataSet::removeFilters()
{
	m_allGroups.clear();
	m_groupRows.clear();
	m_itemRows.clear();
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