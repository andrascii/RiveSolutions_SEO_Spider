#include "summary_data_accessor_decorator.h"

namespace QuickieWebBot
{

SummaryDataAccessorDecorator::SummaryDataAccessorDecorator(ISummaryDataAccessor* dataAccessor)
	: m_summaryDataAccessor(dataAccessor)
{
	VERIFY(connect(dataAccessor->qobject(), SIGNAL(dataChanged(int, int, Qt::ItemDataRole)), this, SIGNAL(dataChanged(int, int, Qt::ItemDataRole))));
}

StorageAdaptorType SummaryDataAccessorDecorator::itemCategory(const QModelIndex& index) const noexcept
{
	return m_summaryDataAccessor->itemCategory(index);
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

void SummaryDataAccessorDecorator::addGroup(AuditGroup group) noexcept
{
	return m_summaryDataAccessor->addGroup(group);
}

const WebCrawler::SequencedDataCollection* SummaryDataAccessorDecorator::guiStorage() const noexcept
{
	return m_summaryDataAccessor->guiStorage();
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

const DCStorageDescription* SummaryDataAccessorDecorator::storageDescription(WebCrawler::StorageType type) const noexcept
{
	return m_summaryDataAccessor->storageDescription(type);
}

const QuickieWebBot::DCStorageGroupDescription* SummaryDataAccessorDecorator::storageGroupDescription(AuditGroup group) const noexcept
{
	return m_summaryDataAccessor->storageGroupDescription(group);
}

}