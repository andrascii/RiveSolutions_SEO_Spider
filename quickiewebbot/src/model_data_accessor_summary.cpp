#include "model_data_accessor_summary.h"
#include "model_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "model_controller.h"
#include "data_collection.h"
#include "grid_view_full_size_resize_strategy.h"


namespace QuickieWebBot
{

ModelDataAccessorSummary::ModelDataAccessorSummary()
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
				SummaryItem{ "Links with non-ASCII characters", ModelDataAccessorFactoryParams::TypeAllNonAsciiLinks, StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", ModelDataAccessorFactoryParams::TypeAllUppercaseLinks, StatusOK, 0 },
				SummaryItem{ "Too long links", ModelDataAccessorFactoryParams::TypeAllVeryLongLinks, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", ModelDataAccessorFactoryParams::TypeAllEmptyTitle, StatusOK, 0 },
				SummaryItem{ "Duplicate titles", ModelDataAccessorFactoryParams::TypeAllDuplicatedTitle, StatusOK, 0 },
				SummaryItem{ "Too long titles", ModelDataAccessorFactoryParams::TypeAllVeryLongTitle, StatusOK, 0 },
				SummaryItem{ "Too short titles", ModelDataAccessorFactoryParams::TypeAllVeryShortTitle, StatusOK, 0 },
				SummaryItem{ "H1 Duplcates titles", ModelDataAccessorFactoryParams::TypeAllDuplicatedH1Title, StatusOK, 0 },
				SummaryItem{ "Several title tags", ModelDataAccessorFactoryParams::TypeAllSeveralTitle, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", ModelDataAccessorFactoryParams::TypeAllEmptyMetaDescription, StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", ModelDataAccessorFactoryParams::TypeAllDuplicatedH1, StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", ModelDataAccessorFactoryParams::TypeAllVeryLongMetaDescription, StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", ModelDataAccessorFactoryParams::TypeAllVeryShortMetaDescription, StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", ModelDataAccessorFactoryParams::TypeAllSeveralMetaDescription, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta key words", ModelDataAccessorFactoryParams::TypeAllEmptyMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Duplicate meta key words", ModelDataAccessorFactoryParams::TypeAllDuplicatedMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Several meta key words tags", ModelDataAccessorFactoryParams::TypeAllSeveralMetaKeywords, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", ModelDataAccessorFactoryParams::TypeAllMissingH1, StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", ModelDataAccessorFactoryParams::TypeAllDuplicatedH1, StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", ModelDataAccessorFactoryParams::TypeAllVeryLongH1, StatusOK, 0 },
				SummaryItem{ "Several H1 tags", ModelDataAccessorFactoryParams::TypeAllSeveralH1, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", ModelDataAccessorFactoryParams::TypeAllMissingH2, StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", ModelDataAccessorFactoryParams::TypeAllDuplicatedH2, StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", ModelDataAccessorFactoryParams::TypeAllVeryLongH2, StatusOK, 0 },
				SummaryItem{ "Several H2 tags", ModelDataAccessorFactoryParams::TypeAllSeveralH2, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", ModelDataAccessorFactoryParams::TypeAllOver100kbImages, StatusOK, 0 },
				SummaryItem{ "Missing alt description", ModelDataAccessorFactoryParams::TypeAllMissingAltImages, StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", ModelDataAccessorFactoryParams::TypeAllVeryLongAltTextImages, StatusOK, 0 },
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

QList<PageInfo::ItemType> ModelDataAccessorSummary::supportedColumns() const
{
	return m_supportedColumns;
}

int ModelDataAccessorSummary::itemCount() const
{
	return static_cast<int>(m_groupByRowRefs.size() + m_itemByRowRefs.size());
}

QVariant ModelDataAccessorSummary::item(const QModelIndex& index) const
{
	auto groupIterator = m_groupByRowRefs.find(index.row());

	if (groupIterator != m_groupByRowRefs.end())
	{
		return groupIterator->second->groupName;
	}

	auto itemIterator = m_itemByRowRefs.find(index.row());

	assert(itemIterator != m_itemByRowRefs.end());

	return index.column() == 0 ? QVariant(itemIterator->second->name) : QVariant(itemIterator->second->issueCount);
}

QVariant ModelDataAccessorSummary::item(int row, int column) const
{
	auto groupIterator = m_groupByRowRefs.find(row);

	if (groupIterator != m_groupByRowRefs.end())
	{
		return groupIterator->second->groupName;
	}

	auto itemIterator = m_itemByRowRefs.find(row);

	assert(itemIterator != m_itemByRowRefs.end());

	return column == 0 ? QVariant(itemIterator->second->name) : QVariant(itemIterator->second->issueCount);
}

PageInfoPtr ModelDataAccessorSummary::pageInfoAtRow(int row) const
{
	assert(!"This type don't implement this call");
	return PageInfoPtr();
}

ModelDataAccessorFactoryParams ModelDataAccessorSummary::childViewParams(const QItemSelection& selection) const
{
	QModelIndexList indicies = selection.indexes();

	if (indicies.isEmpty())
	{
		return ModelDataAccessorFactoryParams(ModelDataAccessorFactoryParams::TypeInvalid);
	}

	QModelIndex index = indicies.first();
	assert(!isGroupHeaderRow(index.row()));
	assert(index.isValid());

	auto itemIt = m_itemByRowRefs.find(index.row());
	assert(itemIt != m_itemByRowRefs.end());

	auto accessorType = static_cast<ModelDataAccessorFactoryParams::Type>(itemIt->second->id);
	return ModelDataAccessorFactoryParams(accessorType, ModelDataAccessorFactoryParams::ModeSummary);
}

IGridViewResizeStrategy* ModelDataAccessorSummary::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

QObject* ModelDataAccessorSummary::qobject()
{
	return this;
}

bool ModelDataAccessorSummary::isGroupHeaderRow(int row) const
{
	return m_groupByRowRefs.find(row) != m_groupByRowRefs.end();
}

QPixmap ModelDataAccessorSummary::pixmapIcon(ItemStatus status, int size) const
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

void ModelDataAccessorSummary::onModelDataRowAdded(int row, int type)
{
	auto it = m_itemByTypeRefs.find(type);
	if (it != m_itemByTypeRefs.end())
	{
		it->second->issueCount++;
		emit itemChanged(it->second->row, 1);

		if (it->second->issueCount == 1)
		{
			// first issue, update icon
			emit itemChanged(it->second->row, 0);
		}
	}
}

}