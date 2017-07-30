#include "grid_data_accessor_summary.h"
#include "grid_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "model_controller.h"
#include "data_collection.h"
#include "grid_view_full_size_resize_strategy.h"


namespace QuickieWebBot
{

GridDataAccessorSummary::GridDataAccessorSummary()
	: m_resizeStrategy(std::make_unique<GridViewFullSizeResizeStrategy>(std::vector<int>{ 60, 40 }))
	, m_backgroundPainter(Qt::transparent, Qt::transparent)
{
	// colspan
	//return m_groupByRowRefs.find(index.row()) != m_groupByRowRefs.end() ? 2 : 1;

	m_groups = 
	{
		SummaryGroup
		{
			"Links",
			{
				SummaryItem{ "Links with non-ASCII characters", GridDataAccessorFactoryParams::TypeAllNonAsciiLinks, StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", GridDataAccessorFactoryParams::TypeAllUppercaseLinks, StatusOK, 0 },
				SummaryItem{ "Too long links", GridDataAccessorFactoryParams::TypeAllVeryLongLinks, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", GridDataAccessorFactoryParams::TypeAllEmptyTitle, StatusOK, 0 },
				SummaryItem{ "Duplicate titles", GridDataAccessorFactoryParams::TypeAllDuplicatedTitle, StatusOK, 0 },
				SummaryItem{ "Too long titles", GridDataAccessorFactoryParams::TypeAllVeryLongTitle, StatusOK, 0 },
				SummaryItem{ "Too short titles", GridDataAccessorFactoryParams::TypeAllVeryShortTitle, StatusOK, 0 },
				SummaryItem{ "H1 Duplcates titles", GridDataAccessorFactoryParams::TypeAllDuplicatedH1Title, StatusOK, 0 },
				SummaryItem{ "Several title tags", GridDataAccessorFactoryParams::TypeAllSeveralTitle, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", GridDataAccessorFactoryParams::TypeAllEmptyMetaDescription, StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", GridDataAccessorFactoryParams::TypeAllDuplicatedH1, StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", GridDataAccessorFactoryParams::TypeAllVeryLongMetaDescription, StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", GridDataAccessorFactoryParams::TypeAllVeryShortMetaDescription, StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", GridDataAccessorFactoryParams::TypeAllSeveralMetaDescription, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta key words", GridDataAccessorFactoryParams::TypeAllEmptyMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Duplicate meta key words", GridDataAccessorFactoryParams::TypeAllDuplicatedMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Several meta key words tags", GridDataAccessorFactoryParams::TypeAllSeveralMetaKeywords, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", GridDataAccessorFactoryParams::TypeAllMissingH1, StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", GridDataAccessorFactoryParams::TypeAllDuplicatedH1, StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", GridDataAccessorFactoryParams::TypeAllVeryLongH1, StatusOK, 0 },
				SummaryItem{ "Several H1 tags", GridDataAccessorFactoryParams::TypeAllSeveralH1, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", GridDataAccessorFactoryParams::TypeAllMissingH2, StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", GridDataAccessorFactoryParams::TypeAllDuplicatedH2, StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", GridDataAccessorFactoryParams::TypeAllVeryLongH2, StatusOK, 0 },
				SummaryItem{ "Several H2 tags", GridDataAccessorFactoryParams::TypeAllSeveralH2, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", GridDataAccessorFactoryParams::TypeAllOver100kbImages, StatusOK, 0 },
				SummaryItem{ "Missing alt description", GridDataAccessorFactoryParams::TypeAllMissingAltImages, StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", GridDataAccessorFactoryParams::TypeAllVeryLongAltTextImages, StatusOK, 0 },
			}
		}
	};

	int modelRowIndex = 0;

	for (auto groupIt = std::begin(m_groups); groupIt != std::end(m_groups); ++groupIt)
	{
		m_groupByRowRefs[modelRowIndex] = &(*groupIt);
		modelRowIndex++;

		for (auto itemIt = std::begin(groupIt->groupItems); itemIt != std::end(groupIt->groupItems); ++itemIt)
		{
			m_itemByRowRefs[modelRowIndex] = &(*itemIt);
			m_itemByTypeRefs[itemIt->id] = &(*itemIt);
			itemIt->row = modelRowIndex;
			modelRowIndex++;
		}
	}

	const DataCollection* modelControllerData = theApp->modelController()->data();
	VERIFY(connect(modelControllerData, SIGNAL(pageInfoAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}

int GridDataAccessorSummary::columnsCount() const
{
	return 2;
}

int GridDataAccessorSummary::itemCount() const
{
	return m_groupByRowRefs.size() + m_itemByRowRefs.size();
}

QVariant GridDataAccessorSummary::columnDescription(int section) const
{
	return QVariant();
}

QVariant GridDataAccessorSummary::item(const QModelIndex& index) const
{
	auto groupIterator = m_groupByRowRefs.find(index.row());

	if (groupIterator != m_groupByRowRefs.end())
	{
		return groupIterator.value()->groupName;
	}

	auto itemIterator = m_itemByRowRefs.find(index.row());

	assert(itemIterator != m_itemByRowRefs.end());

	return index.column() == 0 ? QVariant(itemIterator.value()->name) : QVariant(itemIterator.value()->issueCount);
}

QVariant GridDataAccessorSummary::item(int row, int column) const
{
	auto groupIterator = m_groupByRowRefs.find(row);

	if (groupIterator != m_groupByRowRefs.end())
	{
		return groupIterator.value()->groupName;
	}

	auto itemIterator = m_itemByRowRefs.find(row);

	assert(itemIterator != m_itemByRowRefs.end());

	return column == 0 ? QVariant(itemIterator.value()->name) : QVariant(itemIterator.value()->issueCount);
}

PageInfoPtr GridDataAccessorSummary::pageInfoAtRow(int row) const
{
	assert(!"This type don't implement this call");
	return PageInfoPtr();
}

GridDataAccessorFactoryParams GridDataAccessorSummary::childViewParams(const QItemSelection& selection) const
{
	QModelIndexList indicies = selection.indexes();

	if (indicies.isEmpty())
	{
		return GridDataAccessorFactoryParams(GridDataAccessorFactoryParams::TypeInvalid);
	}

	QModelIndex index = indicies.first();
	assert(!isGroupHeaderRow(index.row()));
	assert(index.isValid());

	auto itemIt = m_itemByRowRefs.find(index.row());
	assert(itemIt != m_itemByRowRefs.end());

	auto accessorType = static_cast<GridDataAccessorFactoryParams::Type>(itemIt.value()->id);
	return GridDataAccessorFactoryParams(accessorType, GridDataAccessorFactoryParams::ModeSummary);
}

IGridViewResizeStrategy* GridDataAccessorSummary::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

QObject* GridDataAccessorSummary::qobject()
{
	return this;
}

bool GridDataAccessorSummary::isGroupHeaderRow(int row) const
{
	return m_groupByRowRefs.find(row) != m_groupByRowRefs.end();
}

QPixmap GridDataAccessorSummary::pixmapIcon(ItemStatus status, int size) const
{
	QMap<ItemStatus, QByteArray> paths
	{
		{ StatusOK, ":/images/icon-ok.svg" },
		{ StatusWarning, ":/images/icon-warning.svg" },
		{ StatusError, ":/images/icon-error.svg" },
	};

	QSvgRenderer renderer(QString(paths.value(status)));

	QPixmap pixmap(size, size);
	pixmap.fill(Qt::transparent);
	QPainter painterPixmap(&pixmap);
	renderer.render(&painterPixmap);

	return pixmap;
}

void GridDataAccessorSummary::onModelDataRowAdded(int row, int type)
{
	auto it = m_itemByTypeRefs.find(type);

	if (it != m_itemByTypeRefs.end())
	{
		it.value()->issueCount++;
		emit itemChanged(it.value()->row, 1);

		if (it.value()->issueCount == 1)
		{
			// first issue, update icon
			emit itemChanged(it.value()->row, 0);
		}
	}
}

}