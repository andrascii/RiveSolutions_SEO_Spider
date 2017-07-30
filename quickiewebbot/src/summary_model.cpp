#include "summary_model.h"
#include "model_controller.h"
#include "data_collection.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{
	
SummaryModel::SummaryModel(QObject* parent)
	: GridModel(parent)
{
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
}

QVariant SummaryModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
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

	if (role == Qt::DecorationRole)
	{
		static QPixmap s_okPixmap = pixmapIcon(StatusOK, QuickieWebBotHelpers::pointsToPixels(13.5));
		static QPixmap s_warningPixmap = pixmapIcon(StatusWarning, QuickieWebBotHelpers::pointsToPixels(13.5));
		static QPixmap s_errorPixmap = pixmapIcon(StatusError, QuickieWebBotHelpers::pointsToPixels(13.5));

		if (isGroupHeaderRow(index.row()) || index.column() != 0)
		{
			return QVariant();
		}

		auto it = m_itemByRowRefs.find(index.row());
		assert(it != m_itemByRowRefs.end());

		return it.value()->issueCount > 0 ? s_warningPixmap : s_okPixmap;
	}

	return QVariant();
}

QVariant SummaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

int SummaryModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

int SummaryModel::rowCount(const QModelIndex& parent) const
{
	return m_groupByRowRefs.size() + m_itemByRowRefs.size();
}

bool SummaryModel::isGroupHeaderRow(int row) const
{
	return m_groupByRowRefs.find(row) != m_groupByRowRefs.end();
}

QPixmap SummaryModel::pixmapIcon(ItemStatus status, int size) const
{
	static QMap<ItemStatus, QByteArray> s_paths
	{
		{ StatusOK, ":/images/icon-ok.svg" },
		{ StatusWarning, ":/images/icon-warning.svg" },
		{ StatusError, ":/images/icon-error.svg" },
	};

	QSvgRenderer renderer(QString(s_paths.value(status)));

	QPixmap pixmap(size, size);
	pixmap.fill(Qt::transparent);

	QPainter painterPixmap(&pixmap);
	renderer.render(&painterPixmap);

	return pixmap;
}

}