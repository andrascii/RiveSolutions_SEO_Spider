#include "summary_data_accessor_decorator.h"
#include "summary_data_accessor.h"

namespace SeoSpider
{

SummaryDataAccessorDecorator::SummaryDataAccessorDecorator(ISummaryDataAccessor* dataAccessor)
	: m_summaryDataAccessor(dataAccessor)
{
	establishSignalToSignalConnections();
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

void SummaryDataAccessorDecorator::establishSignalToSignalConnections()
{
	const auto collectSignals = [](const QMetaObject* metaObject, std::vector<QMetaMethod>& signalsCollection)
	{
		for (int i = 0; i < metaObject->methodCount(); ++i)
		{
			QMetaMethod metaMethod = metaObject->method(i);

			if (metaMethod.methodType() != QMetaMethod::Signal)
			{
				continue;
			}

			signalsCollection.push_back(metaMethod);
		}
	};

	const QMetaObject* thisMetaObject = metaObject();
	const QMetaObject* decoratedDataAccessorMetaObject = m_summaryDataAccessor->qobject()->metaObject();

	std::vector<QMetaMethod> thisSignals;
	std::vector<QMetaMethod> decoratedDataAccessorSignals;

	collectSignals(thisMetaObject, thisSignals);
	collectSignals(decoratedDataAccessorMetaObject, decoratedDataAccessorSignals);

	ASSERT(thisSignals.size() == decoratedDataAccessorSignals.size());

	for (std::size_t i = 0; i < thisSignals.size(); ++i)
	{
		QByteArray methodSignature = thisSignals[i].methodSignature();

		if (methodSignature.contains("destroyed") || methodSignature.contains("objectNameChanged"))
		{
			continue;
		}

		for (std::size_t j = 0; j < decoratedDataAccessorSignals.size(); ++j)
		{
			QByteArray decoratedDataAccessorSignal = decoratedDataAccessorSignals[j].methodSignature();

			if (methodSignature == decoratedDataAccessorSignal)
			{
				QByteArray signal = "2" + methodSignature;

				VERIFY(connect(m_summaryDataAccessor->qobject(), signal, this, signal));
			}
		}
	}
}

}
