#include "summary_data_accessor.h"
#include "application.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"
#include "error_category.h"

namespace SeoSpider
{

SummaryDataAccessor::SummaryDataAccessor(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection,
	const std::function<bool(DCStorageDescription*, DCStorageDescription*)>& sortPredicate)
	: m_sequencedDataCollection(sequencedDataCollection)
	, m_sortPredicate(sortPredicate)
{
	VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageRemoved,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageReplaced,
		this, &SummaryDataAccessor::emitDataChanged));

	if (m_sortPredicate)
	{
		VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded,
			this, &SummaryDataAccessor::sortGroups));
	}

	VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &SummaryDataAccessor::beginClearData));

	VERIFY(connect(m_sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::endClearData,
		this, &SummaryDataAccessor::endClearData));
}

int SummaryDataAccessor::columnCount() const noexcept
{
	return s_summaryColumnCount;
}

int SummaryDataAccessor::rowCount() const noexcept
{
	return m_groupRows.size() + m_itemRows.size();
}

void SummaryDataAccessor::addGroup(AuditGroup group) noexcept
{
	DataCollectionGroupsFactory dcGroupsFactory;
	m_allGroupRows.push_back(dcGroupsFactory.create(group));

	int modelRowIndex = rowCount();
	m_groupRows[modelRowIndex++] = m_allGroupRows.last();

	for (auto itemStart = std::begin(m_allGroupRows.last()->descriptions); 
		itemStart != std::end(m_allGroupRows.last()->descriptions); 
		++itemStart, ++modelRowIndex)
	{
		m_itemRows[modelRowIndex] = &(*itemStart);
	}
}

const CrawlerEngine::SequencedDataCollection* SummaryDataAccessor::sequencedDataCollection() const noexcept
{
	return m_sequencedDataCollection;
}

QObject* SummaryDataAccessor::qobject() noexcept
{
	return this;
}

const DCStorageDescription* SummaryDataAccessor::storageDescriptionByRow(int row) const noexcept
{
	for(auto beg = m_itemRows.begin(); beg != m_itemRows.end(); ++beg)
	{
		if (beg.key() == row)
		{
			return beg.value();
		}
	}

	return nullptr;
}

const DCStorageGroupDescription* SummaryDataAccessor::storageGroupDescriptionByRow(int row) const noexcept
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

const DCStorageDescription* SummaryDataAccessor::storageDescription(CrawlerEngine::StorageType type) const noexcept
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

const DCStorageGroupDescription* SummaryDataAccessor::storageGroupDescription(AuditGroup group) const noexcept
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

Menu SummaryDataAccessor::menuFor(const QModelIndex&) const
{
	return Menu();
}

int SummaryDataAccessor::rowByStorageType(CrawlerEngine::StorageType storageType) const noexcept
{
	for (int i = 0; i < rowCount(); ++i)
	{
		if (!storageDescriptionByRow(i))
		{
			continue;
		}

		if (storageDescriptionByRow(i)->storageType == storageType)
		{
			return i;
		}
	}

	return -1;
}

void SummaryDataAccessor::emitDataChanged(int, CrawlerEngine::StorageType storageType)
{
	const int row = rowByStorageType(storageType);

	if (row == -1)
	{
		return;
	}

	Q_EMIT dataChanged(row, 1, Qt::DisplayRole);
}

void SummaryDataAccessor::sortGroups(int storageRow, CrawlerEngine::StorageType)
{
	//
	// TODO: make it more clever
	//

	if (!storageRow)
	{
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
	}
}

Qt::ItemFlags SummaryDataAccessor::flags(const QModelIndex& index) const noexcept
{
	if (isHeaderRow(index.row()))
	{
		return Qt::ItemIsEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QSize SummaryDataAccessor::span(const QModelIndex& index) const noexcept
{
	return m_groupRows.find(index.row()) != m_groupRows.end() ? QSize(2, 1) : QSize(1, 1);
}

QVariant SummaryDataAccessor::item(const QModelIndex& index) const noexcept
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

bool SummaryDataAccessor::isHeaderRow(int row) const noexcept
{
	return m_groupRows.find(row) != m_groupRows.end();
}

StorageAdapterType SummaryDataAccessor::itemCategory(const QModelIndex& index) const noexcept
{
	if (isHeaderRow(index.row()) || !index.isValid())
	{
		return StorageAdapterType::StorageAdapterTypeNone;
	}

	return StorageAdapterType{ m_itemRows[index.row()]->storageType };
}

const QPixmap& SummaryDataAccessor::pixmap(const QModelIndex& index) const noexcept
{
	Q_UNUSED(index);

	static QPixmap emptyPixmap;
	return emptyPixmap;
}

}
