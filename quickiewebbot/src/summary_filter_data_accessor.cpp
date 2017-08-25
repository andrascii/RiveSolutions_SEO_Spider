#include "summary_filter_data_accessor.h"
#include "quickie_web_bot_helpers.h"
#include "application.h"
#include "model_controller.h"

namespace QuickieWebBot
{

SummaryFilterDataAccessor::SummaryFilterDataAccessor()
{
	DataCollectionGroupsFactory dcGroupsFactory;

	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::LinkProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::TitleProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::MetaDescriptionProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::MetaKeywordProblems));
	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::H1Problems));
	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::H2Problems));
	m_allGroupRows.push_back(dcGroupsFactory.create(theApp->modelController()->data(), ProblemAuditGroups::ImageProblems));

	int modelRowIndex = 0;

	for (auto start = std::begin(m_allGroupRows); start != std::end(m_allGroupRows); ++start)
	{
		m_groupRows[modelRowIndex] = *start;

		++modelRowIndex;

		for (auto itemStart = std::begin((*start)->descriptions); itemStart != std::end((*start)->descriptions); ++itemStart)
		{
			m_itemRows[modelRowIndex] = &(*itemStart);

			++modelRowIndex;
		}
	}
}

int SummaryFilterDataAccessor::columnCount() const noexcept
{
	return s_summaryColumnCount;
}

int SummaryFilterDataAccessor::rowCount() const noexcept
{
	return m_groupRows.size() + m_itemRows.size();
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

	assert(itemIterator != m_itemRows.end());

	if (index.column() == 0)
	{
		return itemIterator.value()->storageTypeDescriptionName;
	}

	return theApp->modelController()->data()->guiStorage(itemIterator.value()->storageType)->size();
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

	return SummaryCategoryItem(m_itemRows[index.row()]->storageType);
}

QPixmap SummaryFilterDataAccessor::pixmap(const QModelIndex& index) const noexcept
{
	if (isHeaderItem(index) || index.column() != 0)
	{
		return QPixmap();
	}

	static QMap<ItemStatus, QByteArray> s_paths
	{
		{ StatusOK, ":/images/icon-ok.svg" },
		{ StatusWarning, ":/images/icon-warning.svg" },
		{ StatusError, ":/images/icon-error.svg" },
	};

	QSvgRenderer renderer(QString(s_paths.value(itemStatus(index))));

	const int etalonPixmapHeightWidth = QuickieWebBotHelpers::pointsToPixels(13.5);

	QPixmap pixmap(QSize(etalonPixmapHeightWidth, etalonPixmapHeightWidth));

	pixmap.fill(Qt::transparent);

	QPainter painterPixmap(&pixmap);

	renderer.render(&painterPixmap);

	return pixmap;
}

SummaryFilterDataAccessor::ItemStatus SummaryFilterDataAccessor::itemStatus(const QModelIndex& index) const noexcept
{
	//
	// TODO
	//
	return StatusOK;
}

}