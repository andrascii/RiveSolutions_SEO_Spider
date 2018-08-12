#include "summary_data_accessor_decorator.h"
#include "summary_data_accessor.h"
#include "helpers.h"

namespace SeoSpider
{

SummaryDataAccessorDecorator::SummaryDataAccessorDecorator(ISummaryDataAccessor* dataAccessor)
	: m_summaryDataAccessor(dataAccessor)
{
	Common::Helpers::connectSignalsToMetaMethodsWithTheSameName(
		m_summaryDataAccessor->qobject(),
		Common::Helpers::allUserSignals(m_summaryDataAccessor->qobject()),
		this,
		Common::Helpers::allUserSignals(this)
	);
}

void SummaryDataAccessorDecorator::setSortableDataSet(SummaryDataSet* dataSet) noexcept
{
	m_summaryDataAccessor->setSortableDataSet(dataSet);
}

void SummaryDataAccessorDecorator::enableSortableDataSet() noexcept
{
	m_summaryDataAccessor->enableSortableDataSet();
}

void SummaryDataAccessorDecorator::enablePlainDataSet() noexcept
{
	m_summaryDataAccessor->enablePlainDataSet();
}

bool SummaryDataAccessorDecorator::hasSortableDataSet() const noexcept
{
	return m_summaryDataAccessor->hasSortableDataSet();
}

StorageAdapterType SummaryDataAccessorDecorator::itemCategory(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->itemCategory(index);
}

QString SummaryDataAccessorDecorator::customDataFeed(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->customDataFeed(index);
}

Qt::ItemFlags SummaryDataAccessorDecorator::flags(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->flags(index);
}

bool SummaryDataAccessorDecorator::isHeaderRow(int row) const noexcept
{
	return m_summaryDataAccessor->isHeaderRow(row);
}

const QPixmap& SummaryDataAccessorDecorator::pixmap(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->pixmap(index);
}

QVariant SummaryDataAccessorDecorator::item(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->item(index);
}

QSize SummaryDataAccessorDecorator::span(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->span(index);
}

int SummaryDataAccessorDecorator::columnCount() const noexcept
{
	return m_summaryDataAccessor->columnCount();
}

int SummaryDataAccessorDecorator::rowCount() const noexcept
{
	return m_summaryDataAccessor->rowCount();
}

const CrawlerEngine::SequencedDataCollection* SummaryDataAccessorDecorator::sequencedDataCollection() const noexcept
{
	return m_summaryDataAccessor->sequencedDataCollection();
}

QObject* SummaryDataAccessorDecorator::qobject() noexcept
{
	return this;
}

const DCStorageDescription* SummaryDataAccessorDecorator::storageDescriptionByRow(int row) const noexcept
{
	return m_summaryDataAccessor->storageDescriptionByRow(row);
}

const DCStorageGroupDescription* SummaryDataAccessorDecorator::storageGroupDescriptionByRow(int row) const noexcept
{
	return m_summaryDataAccessor->storageGroupDescriptionByRow(row);
}

const DCStorageDescription* SummaryDataAccessorDecorator::storageDescription(CrawlerEngine::StorageType type) const noexcept
{
	return m_summaryDataAccessor->storageDescription(type);
}

const DCStorageGroupDescription* SummaryDataAccessorDecorator::storageGroupDescription(AuditGroup group) const noexcept
{
	return m_summaryDataAccessor->storageGroupDescription(group);
}

Menu SummaryDataAccessorDecorator::menuFor(const QModelIndex& index) const
{
	return m_summaryDataAccessor->menuFor(index);
}

void SummaryDataAccessorDecorator::selectRow(int row) noexcept
{
	return m_summaryDataAccessor->selectRow(row);
}

int SummaryDataAccessorDecorator::selectedRow() const noexcept
{
	return m_summaryDataAccessor->selectedRow();
}

int SummaryDataAccessorDecorator::rowByStorageType(CrawlerEngine::StorageType storageType) const noexcept
{
	return m_summaryDataAccessor->rowByStorageType(storageType);
}

}
