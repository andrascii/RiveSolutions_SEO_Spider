#include "summary_data_accessor.h"
#include "quickie_web_bot_helpers.h"
#include "application.h"
#include "model_controller.h"

namespace QuickieWebBot
{

SummaryDataAccessor::SummaryDataAccessor(WebCrawler::DataCollection* dataCollection)
	: m_dataCollection(dataCollection)
{
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
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, group));

	int modelRowIndex = rowCount();
	m_groupRows[modelRowIndex++] = m_allGroupRows.last();

	for (auto itemStart = std::begin(m_allGroupRows.last()->descriptions); 
		itemStart != std::end(m_allGroupRows.last()->descriptions); 
		++itemStart, ++modelRowIndex)
	{
		m_itemRows[modelRowIndex] = &(*itemStart);
	}
}

const WebCrawler::DataCollection* SummaryDataAccessor::dataCollection() const noexcept
{
	return m_dataCollection;
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

const DCStorageDescription* SummaryDataAccessor::storageDescription(WebCrawler::DataCollection::StorageType type) const noexcept
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

	return m_dataCollection->guiStorage(itemIterator.value()->storageType)->size();
}

bool SummaryDataAccessor::isHeaderRow(int row) const noexcept
{
	return m_groupRows.find(row) != m_groupRows.end();
}

SummaryCategoryItem SummaryDataAccessor::itemCategory(const QModelIndex& index) const noexcept
{
	if (isHeaderRow(index.row()))
	{
		return SummaryCategoryItem::SummaryCategoryItemHeader;
	}

	return SummaryCategoryItem{ m_itemRows[index.row()]->storageType };
}

const QPixmap& SummaryDataAccessor::pixmap(const QModelIndex& index) const noexcept
{
	static QPixmap emptyPixmap;

	return emptyPixmap;
}

}