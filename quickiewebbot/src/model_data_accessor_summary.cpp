#include "model_data_accessor_summary.h"
#include "grid_view_painter_text.h"
#include "model_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "model_controller.h"
#include "data_collection.h"

namespace QuickieWebBot
{

ModelDataAccessorSummary::ModelDataAccessorSummary()
{
	using FP = QuickieWebBot::ModelDataAccessorFactoryParams;

	m_groups = 
	{
		SummaryGroup
		{
			"Links",
			{
				SummaryItem{ "Links with non-ASCII characters", FP::TypeSummaryLinksNonAscii, StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", FP::TypeSummaryLinksUpperCase, StatusOK, 0 },
				SummaryItem{ "Too long links", FP::TypeSummaryLinksTooLong, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", FP::TypeSummaryTitlesEmpty, StatusOK, 0 },
				SummaryItem{ "Duplicate titles", FP::TypeSummaryTitlesDuplicates, StatusOK, 0 },
				SummaryItem{ "Too long titles", FP::TypeSummaryTitlesTooLong, StatusOK, 0 },
				SummaryItem{ "Too short titles", FP::TypeSummaryTitlesTooShort, StatusOK, 0 },
				SummaryItem{ "H1 Duplcates titles", FP::TypeSummaryTitlesH1Duplicates, StatusOK, 0 },
				SummaryItem{ "Several title tags", FP::TypeSummaryTitlesSeveralTags, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", FP::TypeSummaryMetaDescriptionsEmpty, StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", FP::TypeSummaryMetaDescriptionsDuplicates, StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", FP::TypeSummaryMetaDescriptionsTooLong, StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", FP::TypeSummaryMetaDescriptionsTooShort, StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", FP::TypeSummaryMetaDescriptionsSeveralTags, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta key words", FP::TypeSummaryMetaKeyWordsEmpty, StatusOK, 0 },
				SummaryItem{ "Duplicate meta key words", FP::TypeSummaryMetaKeyWordsDuplicates, StatusOK, 0 },
				SummaryItem{ "Several meta key words tags", FP::TypeSummaryMetaKeyWordsSeveralTags, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", FP::TypeSummaryH1Missing, StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", FP::TypeSummaryH1Duplicates, StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", FP::TypeSummaryH1TooLong, StatusOK, 0 },
				SummaryItem{ "Several H1 tags", FP::TypeSummaryH1SeveralTags, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", FP::TypeSummaryH2Missing, StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", FP::TypeSummaryH2Duplicates, StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", FP::TypeSummaryH2TooLong, StatusOK, 0 },
				SummaryItem{ "Several H2 tags", FP::TypeSummaryH2SeveralTags, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", FP::TypeSummaryImagesOver100KB, StatusOK, 0 },
				SummaryItem{ "Missing alt description", FP::TypeSummaryImagesAltMissing, StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", FP::TypeSummaryImagesAltTooLong, StatusOK, 0 },
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
	auto groupIt = m_groupByRowRefs.find(index.row());
	if (groupIt != m_groupByRowRefs.end())
	{
		return groupIt->second->groupName;
	}

	auto itemIt = m_itemByRowRefs.find(index.row());
	assert(itemIt != m_itemByRowRefs.end());
	return index.column() == 0 ? QVariant(itemIt->second->name) : QVariant(itemIt->second->issueCount);
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
	// TODO: implement
	// TODO: get pixels from em
	static QPixmap s_okPixmap = getPixmapIcon(StatusOK, QuickieWebBotHelpers::pointsToPixels(13.5));
	//static QPixmap s_warningPixmap = getPixmapIcon(StatusWarning, QuickieWebBotHelpers::pointsToPixels(13.5));
	//static QPixmap s_errorPixmap = getPixmapIcon(StatusError, QuickieWebBotHelpers::pointsToPixels(13.5));

	return !isGroupHeaderRow(index.row()) && index.column() == 0 ? &s_okPixmap : nullptr;
}

QObject* ModelDataAccessorSummary::qobject()
{
	return this;
}

ModelDataAccessorFactoryParams ModelDataAccessorSummary::childViewParams(const QItemSelection& selection) const
{
	QModelIndexList indicies = selection.indexes();
	if (indicies.isEmpty())
	{
		return ModelDataAccessorFactoryParams::TypeInvalid;
	}

	QModelIndex index = indicies.first();
	assert(!isGroupHeaderRow(index.row()));
	assert(index.isValid());

	auto itemIt = m_itemByRowRefs.find(index.row());
	assert(itemIt != m_itemByRowRefs.end());

	auto accessorType = static_cast<ModelDataAccessorFactoryParams::Type>(itemIt->second->id);
	return ModelDataAccessorFactoryParams(accessorType);
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
	}
}

}