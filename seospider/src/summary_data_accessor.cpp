#include "summary_data_accessor.h"
#include "application.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"
#include "error_category.h"
#include "summary_data_set.h"

namespace SeoSpider
{

SummaryDataAccessor::SummaryDataAccessor(SummaryDataSet* dataSet)
	: m_currentDataSet(dataSet)
	, m_dataSet(dataSet)
	, m_sortableDataSet(nullptr)
	, m_selectedRow(std::make_pair(-1, CrawlerEngine::StorageType::BeginEnumStorageType))
{
	m_currentDataSet->setParent(this);

	VERIFY(connect(m_currentDataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::parsedPageAdded,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_currentDataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::parsedPageRemoved,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_currentDataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::parsedPageReplaced,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_currentDataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &SummaryDataAccessor::beginClearData));

	VERIFY(connect(m_currentDataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::endClearData,
		this, &SummaryDataAccessor::endClearData));

	VERIFY(connect(m_currentDataSet, &SummaryDataSet::dataSetChanged, this, &SummaryDataAccessor::onDataSetChanged));
}

void SummaryDataAccessor::setSortableDataSet(SummaryDataSet* dataSet) noexcept
{
	if (m_sortableDataSet)
	{
		disconnect(m_sortableDataSet, &SummaryDataSet::sortingStarted, this, &SummaryDataAccessor::beginClearData);
		disconnect(m_sortableDataSet, &SummaryDataSet::sortingEnded, this, &SummaryDataAccessor::endClearData);
		disconnect(m_sortableDataSet, &SummaryDataSet::dataSetChanged, this, &SummaryDataAccessor::onDataSetChanged);
	}

	m_sortableDataSet = dataSet;

	for (int i = 0; i < m_currentDataSet->rowCount(); ++i)
	{
		const DCStorageDescription* currentDataSetStorageDescription =
			m_currentDataSet->storageDescriptionByRow(i);

		if (!currentDataSetStorageDescription)
		{
			continue;
		}

		const DCStorageDescription* sortableDataSetStorageDescription =
			m_sortableDataSet->storageDescription(currentDataSetStorageDescription->storageType);

		ASSERT(sortableDataSetStorageDescription);
	}

	VERIFY(connect(m_sortableDataSet, &SummaryDataSet::sortingStarted, this, &SummaryDataAccessor::beginClearData));
	VERIFY(connect(m_sortableDataSet, &SummaryDataSet::sortingEnded, this, &SummaryDataAccessor::endClearData));
	VERIFY(connect(m_sortableDataSet, &SummaryDataSet::sortingEnded, this, &SummaryDataAccessor::validateSelectedRow));
	VERIFY(connect(m_sortableDataSet, &SummaryDataSet::dataSetChanged, this, &SummaryDataAccessor::onDataSetChanged));

	// VERIFY(connect(this, &SummaryDataAccessor::endClearData, this, &SummaryDataAccessor::restoreSelection));
	VERIFY(connect(this, SIGNAL(endClearData()), this, SLOT(restoreSelection())));
}

void SummaryDataAccessor::enableSortableDataSet() noexcept
{
	if (m_currentDataSet != m_sortableDataSet)
	{
		emit beginClearData();

		m_currentDataSet = m_sortableDataSet;

		emit endClearData();
		emit dataSetChanged();

		validateSelectedRow();
	}
}

void SummaryDataAccessor::enablePlainDataSet() noexcept
{
	if (m_currentDataSet != m_dataSet)
	{
		emit beginClearData();

		m_currentDataSet = m_dataSet;

		emit endClearData();
		emit dataSetChanged();

		validateSelectedRow();
	}
}

bool SummaryDataAccessor::hasSortableDataSet() const noexcept
{
	return m_sortableDataSet != nullptr;
}

int SummaryDataAccessor::columnCount() const noexcept
{
	return m_currentDataSet->columnCount();
}

int SummaryDataAccessor::rowCount() const noexcept
{
	return m_currentDataSet->rowCount();
}

const CrawlerEngine::SequencedDataCollection* SummaryDataAccessor::sequencedDataCollection() const noexcept
{
	return m_currentDataSet->sequencedDataCollection();
}

QObject* SummaryDataAccessor::qobject() noexcept
{
	return this;
}

const DCStorageDescription* SummaryDataAccessor::storageDescriptionByRow(int row) const noexcept
{
	return m_currentDataSet->storageDescriptionByRow(row);
}

const DCStorageGroupDescription* SummaryDataAccessor::storageGroupDescriptionByRow(int row) const noexcept
{
	return m_currentDataSet->storageGroupDescriptionByRow(row);
}

const DCStorageDescription* SummaryDataAccessor::storageDescription(CrawlerEngine::StorageType type) const noexcept
{
	return m_currentDataSet->storageDescription(type);
}

const DCStorageGroupDescription* SummaryDataAccessor::storageGroupDescription(AuditGroup group) const noexcept
{
	return m_currentDataSet->storageGroupDescription(group);
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

void SummaryDataAccessor::selectRow(int row) noexcept
{
	if (row == -1)
	{
		m_selectedRow.first = row;
		m_selectedRow.second = CrawlerEngine::StorageType::BeginEnumStorageType;

		return;
	}

	ASSERT(!isHeaderRow(row));

	const DCStorageDescription* storageDescription = m_currentDataSet->storageDescriptionByRow(row);

	ASSERT(storageDescription);

	m_selectedRow.first = row;
	m_selectedRow.second = storageDescription->storageType;

	emit rowSelected(m_selectedRow.first);
}

int SummaryDataAccessor::selectedRow() const noexcept
{
	return m_selectedRow.first;
}

void SummaryDataAccessor::emitDataChanged(int, CrawlerEngine::StorageType storageType)
{
	const int row = rowByStorageType(storageType);

	if (row == -1)
	{
		return;
	}

	emit dataChanged(row, 1, Qt::DisplayRole);
}

void SummaryDataAccessor::validateSelectedRow()
{
	if (m_selectedRow.first == -1)
	{
		return;
	}

	const int row = rowByStorageType(m_selectedRow.second);

	if (row == -1)
	{
		return;
	}

	selectRow(row);
}

void SummaryDataAccessor::restoreSelection()
{
	emit rowSelected(m_selectedRow.first);
}

void SummaryDataAccessor::onDataSetChanged()
{
	emit beginClearData();
	emit endClearData();
	emit dataSetChanged();
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
	return m_currentDataSet->span(index);
}

QVariant SummaryDataAccessor::item(const QModelIndex& index) const noexcept
{
	return m_currentDataSet->item(index);
}

bool SummaryDataAccessor::isHeaderRow(int row) const noexcept
{
	return m_currentDataSet->isHeaderRow(row);
}

StorageAdapterType SummaryDataAccessor::itemCategory(const QModelIndex& index) const noexcept
{
	return m_currentDataSet->itemCategory(index);
}

QString SummaryDataAccessor::customDataFeed(const QModelIndex& index) const noexcept
{
	return m_currentDataSet->customDataFeed(index);
}

const QPixmap& SummaryDataAccessor::pixmap(const QModelIndex& index) const noexcept
{
	Q_UNUSED(index);

	static QPixmap emptyPixmap;
	return emptyPixmap;
}

}
