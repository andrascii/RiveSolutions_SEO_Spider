#include "summary_data_accessor.h"
#include "application.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"
#include "error_category.h"
#include "summary_data_set.h"

namespace SeoSpider
{

SummaryDataAccessor::SummaryDataAccessor(SummaryDataSet* dataSet)
	: m_dataSet(dataSet) 
	, m_selectedRow(-1)
{
	m_dataSet->setParent(this);

	VERIFY(connect(m_dataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::parsedPageAdded,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_dataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::parsedPageRemoved,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_dataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::parsedPageReplaced,
		this, &SummaryDataAccessor::emitDataChanged));

	VERIFY(connect(m_dataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::beginClearData,
		this, &SummaryDataAccessor::beginClearData));

	VERIFY(connect(m_dataSet->sequencedDataCollection(), &CrawlerEngine::SequencedDataCollection::endClearData,
		this, &SummaryDataAccessor::endClearData));

	VERIFY(connect(m_dataSet, &SummaryDataSet::sortingStarted, this, &SummaryDataAccessor::beginClearData));
	VERIFY(connect(m_dataSet, &SummaryDataSet::sortingEnded, this, &SummaryDataAccessor::endClearData));
}

int SummaryDataAccessor::columnCount() const noexcept
{
	return m_dataSet->columnCount();
}

int SummaryDataAccessor::rowCount() const noexcept
{
	return m_dataSet->rowCount();
}

void SummaryDataAccessor::addGroup(AuditGroup group) noexcept
{
	m_dataSet->addGroup(group);
}

const CrawlerEngine::SequencedDataCollection* SummaryDataAccessor::sequencedDataCollection() const noexcept
{
	return m_dataSet->sequencedDataCollection();
}

QObject* SummaryDataAccessor::qobject() noexcept
{
	return this;
}

const DCStorageDescription* SummaryDataAccessor::storageDescriptionByRow(int row) const noexcept
{
	return m_dataSet->storageDescriptionByRow(row);
}

const DCStorageGroupDescription* SummaryDataAccessor::storageGroupDescriptionByRow(int row) const noexcept
{
	return m_dataSet->storageGroupDescriptionByRow(row);
}

const DCStorageDescription* SummaryDataAccessor::storageDescription(CrawlerEngine::StorageType type) const noexcept
{
	return m_dataSet->storageDescription(type);
}

const DCStorageGroupDescription* SummaryDataAccessor::storageGroupDescription(AuditGroup group) const noexcept
{
	return m_dataSet->storageGroupDescription(group);
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
	if (m_selectedRow == row)
	{
		return;
	}

	ASSERT(!isHeaderRow(row));

	m_selectedRow = row;

	emit rowSelected(m_selectedRow);
}

int SummaryDataAccessor::selectedRow() const noexcept
{
	return m_selectedRow;
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
	return m_dataSet->span(index);
}

QVariant SummaryDataAccessor::item(const QModelIndex& index) const noexcept
{
	return m_dataSet->item(index);
}

bool SummaryDataAccessor::isHeaderRow(int row) const noexcept
{
	return m_dataSet->isHeaderRow(row);
}

StorageAdapterType SummaryDataAccessor::itemCategory(const QModelIndex& index) const noexcept
{
	return m_dataSet->itemCategory(index);
}

const QPixmap& SummaryDataAccessor::pixmap(const QModelIndex& index) const noexcept
{
	Q_UNUSED(index);

	static QPixmap emptyPixmap;
	return emptyPixmap;
}

}
