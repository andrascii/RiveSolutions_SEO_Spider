#include "model_data_accessor_summary.h"
#include "grid_view_painter_text.h"
#include "model_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "model_controller.h"
#include "data_collection.h"
#include "grid_view_full_size_resize_strategy.h"

namespace QuickieWebBot
{

ModelDataAccessorSummary::ModelDataAccessorSummary()
	: m_resizeStrategy(std::make_unique<GridViewFullSizeResizeStrategy>(std::vector<int>{ 60, 40 }))
{
	using FP = QuickieWebBot::ModelDataAccessorFactoryParams;

	m_groups = 
	{
		SummaryGroup
		{
			"Links",
			{
				SummaryItem{ "Links with non-ASCII characters", FP::TypeAllNonAsciiLinks, StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", FP::TypeAllUppercaseLinks, StatusOK, 0 },
				SummaryItem{ "Too long links", FP::TypeAllVeryLongLinks, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", FP::TypeAllEmptyTitle, StatusOK, 0 },
				SummaryItem{ "Duplicate titles", FP::TypeAllDuplicatedTitle, StatusOK, 0 },
				SummaryItem{ "Too long titles", FP::TypeAllVeryLongTitle, StatusOK, 0 },
				SummaryItem{ "Too short titles", FP::TypeAllVeryShortTitle, StatusOK, 0 },
				SummaryItem{ "H1 Duplcates titles", FP::TypeAllDuplicatedH1Title, StatusOK, 0 },
				SummaryItem{ "Several title tags", FP::TypeAllSeveralTitle, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", FP::TypeAllEmptyMetaDescription, StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", FP::TypeAllDuplicatedH1, StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", FP::TypeAllVeryLongMetaDescription, StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", FP::TypeAllVeryShortMetaDescription, StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", FP::TypeAllSeveralMetaDescription, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta key words", FP::TypeAllEmptyMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Duplicate meta key words", FP::TypeAllDuplicatedMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Several meta key words tags", FP::TypeAllSeveralMetaKeywords, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", FP::TypeAllMissingH1, StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", FP::TypeAllDuplicatedH1, StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", FP::TypeAllVeryLongH1, StatusOK, 0 },
				SummaryItem{ "Several H1 tags", FP::TypeAllSeveralH1, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", FP::TypeAllMissingH2, StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", FP::TypeAllDuplicatedH2, StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", FP::TypeAllVeryLongH2, StatusOK, 0 },
				SummaryItem{ "Several H2 tags", FP::TypeAllSeveralH2, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", FP::TypeAllOver100kbImages, StatusOK, 0 },
				SummaryItem{ "Missing alt description", FP::TypeAllMissingAltImages, StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", FP::TypeAllVeryLongAltTextImages, StatusOK, 0 },
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

	DataCollection* modelControllerData = theApp->modelController()->data();
	VERIFY(QObject::connect(modelControllerData, SIGNAL(pageInfoAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}

int ModelDataAccessorSummary::columnCount() const
{
	return 2;
}

QString ModelDataAccessorSummary::columnText(int column) const
{
	return QString();
}

int ModelDataAccessorSummary::rowCount() const
{
	return static_cast<int>(m_groupByRowRefs.size() + m_itemByRowRefs.size());
}

QVariant ModelDataAccessorSummary::itemValue(const QModelIndex& index) const
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

QVariant ModelDataAccessorSummary::itemValue(int row, int column) const
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

QColor ModelDataAccessorSummary::itemBackgroundColor(const QModelIndex& index) const
{
	return QColor();
}

int ModelDataAccessorSummary::itemColSpan(const QModelIndex& index) const
{
	return m_groupByRowRefs.find(index.row()) != m_groupByRowRefs.end() ? 2 : 1;
}

int ModelDataAccessorSummary::flags(const QModelIndex& index) const
{
	bool group = isGroupHeaderRow(index.row());
	int result = group ? ItemFlagTextBold | ItemFlagNotSelectable : ItemFlagNone;
	if (index.column() != 0)
	{
		result |= ItemFlagAlignRight;
	}
	else if (!group)
	{
		result |= ItemFlagTextDecorator;
	}

	return result;
}

QPixmap* ModelDataAccessorSummary::pixmap(const QModelIndex& index) const
{
	static QPixmap s_okPixmap = getPixmapIcon(StatusOK, QuickieWebBotHelpers::pointsToPixels(13.5));
	static QPixmap s_warningPixmap = getPixmapIcon(StatusWarning, QuickieWebBotHelpers::pointsToPixels(13.5));
	//static QPixmap s_errorPixmap = getPixmapIcon(StatusError, QuickieWebBotHelpers::pointsToPixels(13.5));

	if (isGroupHeaderRow(index.row()) || index.column() != 0)
	{
		return nullptr;
	}

	auto it = m_itemByRowRefs.find(index.row());
	assert(it != m_itemByRowRefs.end());
	
	return it->second->issueCount > 0 ? &s_warningPixmap : &s_okPixmap;
}

QObject* ModelDataAccessorSummary::qobject()
{
	return this;
}

ModelDataAccessorFactoryParams ModelDataAccessorSummary::childViewParams(const QItemSelection& selection) const
{
	using FP = ModelDataAccessorFactoryParams;
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

std::vector<GridViewPainter*> ModelDataAccessorSummary::painters(const QModelIndex & index) const
{
	static GridViewPainterText s_textPainter;
	return { &s_textPainter };
}

bool ModelDataAccessorSummary::isGroupHeaderRow(int row) const
{
	return m_groupByRowRefs.find(row) != m_groupByRowRefs.end();
}

QPixmap ModelDataAccessorSummary::getPixmapIcon(ItemStatus status, int size) const
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