#include "summary_filter_data_accessor.h"
#include "quickie_web_bot_helpers.h"
#include "application.h"
#include "model_controller.h"

namespace QuickieWebBot
{

SummaryFilterDataAccessor::SummaryFilterDataAccessor(WebCrawler::DataCollection* dataCollection)
	: m_dataCollection(dataCollection)
	, m_okPixmap(QuickieWebBotHelpers::pointsToPixels(13.5), QuickieWebBotHelpers::pointsToPixels(13.5))
	, m_warningPixmap(QuickieWebBotHelpers::pointsToPixels(13.5), QuickieWebBotHelpers::pointsToPixels(13.5))
	, m_errorPixmap(QuickieWebBotHelpers::pointsToPixels(13.5), QuickieWebBotHelpers::pointsToPixels(13.5))
{
	initializePixmaps();

	DataCollectionGroupsFactory dcGroupsFactory;

	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::LinkProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::TitleProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::MetaDescriptionProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::MetaKeywordProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::H1Problems));
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::H2Problems));
	m_allGroupRows.push_back(dcGroupsFactory.create(m_dataCollection, ProblemAuditGroups::ImageProblems));

	int modelRowIndex = 0;

	for (auto start = std::begin(m_allGroupRows); start != std::end(m_allGroupRows); ++start)
	{
		m_groupRows[modelRowIndex] = *start;

		++modelRowIndex;

		for (auto itemStart = std::begin((*start)->descriptions); itemStart != std::end((*start)->descriptions); ++itemStart)
		{
			m_itemRows[modelRowIndex] = qMakePair(&m_okPixmap, &(*itemStart));

			++modelRowIndex;
		}
	}

	VERIFY(connect(m_dataCollection, SIGNAL(pageRawAdded(int, int)), this, SLOT(emitDataChanged(int, int))));
}

int SummaryFilterDataAccessor::columnCount() const noexcept
{
	return s_summaryColumnCount;
}

int SummaryFilterDataAccessor::rowCount() const noexcept
{
	return m_groupRows.size() + m_itemRows.size();
}

QObject* SummaryFilterDataAccessor::qobject() noexcept
{
	return this;
}

Qt::ItemFlags SummaryFilterDataAccessor::flags(const QModelIndex& index) const noexcept
{
	if (isHeaderItem(index))
	{
		return Qt::ItemIsEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QSize SummaryFilterDataAccessor::span(const QModelIndex& index) const noexcept
{
	return m_groupRows.find(index.row()) != m_groupRows.end() ? QSize(2, 1) : QSize(1, 1);
}

QVariant SummaryFilterDataAccessor::item(const QModelIndex& index) const noexcept
{
	if (isHeaderItem(index))
	{
		return m_groupRows.find(index.row()).value()->name;
	}

	auto itemIterator = m_itemRows.find(index.row());

	ASSERT(itemIterator != m_itemRows.end());

	if (index.column() == 0)
	{
		return itemIterator.value().second->storageTypeDescriptionName;
	}

	return m_dataCollection->guiStorage(itemIterator.value().second->storageType)->size();
}

bool SummaryFilterDataAccessor::isHeaderItem(const QModelIndex& index) const noexcept
{
	return m_groupRows.find(index.row()) != m_groupRows.end();
}

SummaryCategoryItem SummaryFilterDataAccessor::itemCategory(const QModelIndex& index) const noexcept
{
	if (isHeaderItem(index))
	{
		return SummaryCategoryItem::SummaryCategoryItemHeader;
	}

	return SummaryCategoryItem{ m_itemRows[index.row()].second->storageType };
}

const QPixmap& SummaryFilterDataAccessor::pixmap(const QModelIndex& index) const noexcept
{
	static QPixmap emptyPixmap;

	if (isHeaderItem(index) || index.column() != 0)
	{
		return emptyPixmap;
	}

	auto storageDescriptionIter = m_itemRows.find(index.row());

	if (storageDescriptionIter == std::end(m_itemRows))
	{
		DEBUG_ASSERT(!"This code must not be executed because above already exists if header item check");
	}

	return *storageDescriptionIter.value().first;
}

SummaryFilterDataAccessor::ItemStatus SummaryFilterDataAccessor::itemStatus(int row) const noexcept
{
	WebCrawler::DataCollection::GuiStorageTypePtr storage = storageByRow(row);

	ASSERT(storage);

	if (storage->size() > 10)
	{
		return StatusError;
	}
	
	if (storage->size())
	{
		return StatusWarning;
	}

	return StatusOK;
}

int SummaryFilterDataAccessor::rowByStorageType(WebCrawler::DataCollection::StorageType storageType) const noexcept
{
	for(auto beg = m_itemRows.begin(); beg != m_itemRows.end(); ++beg)
	{
		if (beg.value().second->storageType == storageType)
		{
			return beg.key();
		}
	}

	return -1;
}

WebCrawler::DataCollection::GuiStorageTypePtr 
SummaryFilterDataAccessor::storageByRow(int row) const noexcept
{
	for (auto beg = m_itemRows.begin(); beg != m_itemRows.end(); ++beg)
	{
		if (beg.key() == row)
		{
			return m_dataCollection->guiStorage(beg.value().second->storageType);
		}
	}

	return nullptr;
}

void SummaryFilterDataAccessor::changeItemPixmapIfNeeded(int row) noexcept
{
	ASSERT(m_itemRows.find(row) != std::end(m_itemRows));


	static std::map<ItemStatus, QPixmap*> s_pixmaps
	{
		{ StatusOK, &m_okPixmap },
		{ StatusWarning, &m_warningPixmap },
		{ StatusError, &m_errorPixmap }
	};

	auto storageDescriptionIter = m_itemRows.find(row);

	QPixmap* currentItemPixmap = storageDescriptionIter.value().first;

	if (!currentItemPixmap || currentItemPixmap == s_pixmaps[itemStatus(row)])
	{
		return;
	}

	storageDescriptionIter.value().first = s_pixmaps[itemStatus(row)];
	Q_EMIT dataChanged(row, 0, Qt::DecorationRole);
}

void SummaryFilterDataAccessor::initializePixmaps()
{
	QVector<QString> paths
	{
		{ QStringLiteral(":/images/icon-ok.svg") },
		{ QStringLiteral(":/images/icon-warning.svg") },
		{ QStringLiteral(":/images/icon-error.svg") },
	};

	QVector<QPixmap*> pixmaps
	{
		&m_okPixmap,
		&m_warningPixmap,
		&m_errorPixmap
	};

	QSvgRenderer svgRenderer;

	for (int i = 0; i < paths.size(); ++i)
	{
		pixmaps[i]->fill(Qt::transparent);
		QPainter painterPixmap(pixmaps[i]);

		svgRenderer.load(paths[i]);
		svgRenderer.render(&painterPixmap);
	}
}

void SummaryFilterDataAccessor::emitDataChanged(int row, int storageType)
{
	int storageTypeRow = rowByStorageType(static_cast<WebCrawler::DataCollection::StorageType>(storageType));

	if (storageTypeRow == -1)
	{
		return;
	}

	Q_EMIT dataChanged(storageTypeRow, 1, Qt::DisplayRole);
	changeItemPixmapIfNeeded(storageTypeRow);
}

}