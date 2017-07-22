#include "model_data_accessor_summary.h"
#include "grid_view_painter_text.h"

namespace QuickieWebBot
{

ModelDataAccessorSummary::ModelDataAccessorSummary()
{
	m_groups = 
	{
		SummaryGroup
		{
			"Links",
			{
				SummaryItem{ "Links with non-ASCII characters", "links_non_ascii", StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", "links_uppercase", StatusOK, 0 },
				SummaryItem{ "Too long links", "links_too_long", StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", "titles_empty", StatusOK, 0 },
				SummaryItem{ "Duplicate titles", "titles_duplicate", StatusOK, 0 },
				SummaryItem{ "Too long titles", "titles_too_long", StatusOK, 0 },
				SummaryItem{ "Too short titles", "titles_too_short", StatusOK, 0 },
				SummaryItem{ "H1 Duplcates titles", "titles_h1_duplicates", StatusOK, 0 },
				SummaryItem{ "Several title tags", "titles_several_tags", StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", "metadescriptions_empty", StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", "metadescriptions_duplicate", StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", "metadescriptions_too_long", StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", "metadescriptions_too_short", StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", "metadescriptions_several_tags", StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta key words", "metakeywords_empty", StatusOK, 0 },
				SummaryItem{ "Duplicate meta key words", "metakeywords_duplicate", StatusOK, 0 },
				SummaryItem{ "Several meta key words tags", "metakeywords_several_tags", StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", "h1_missing", StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", "h1_duplicate", StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", "h1_too_long", StatusOK, 0 },
				SummaryItem{ "Several H1 tags", "h1_several_tags", StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", "h2_missing", StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", "h2_duplicate", StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", "h2_too_long", StatusOK, 0 },
				SummaryItem{ "Several H2 tags", "h2_several_tags", StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", "images_over_100kb", StatusOK, 0 },
				SummaryItem{ "Missing alt description", "images_alt_missing", StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", "images_alt_too_long", StatusOK, 0 },
			}
		}
	};

	int modelRowIndex = 0;
	for (auto groupIt = std::begin(m_groups); groupIt != std::end(m_groups); ++groupIt)
	{
		m_groupRefs[modelRowIndex] = &(*groupIt);
		modelRowIndex++;
		for (auto itemIt = std::begin(groupIt->groupItems); itemIt != std::end(groupIt->groupItems); ++itemIt)
		{
			m_itemRefs[modelRowIndex] = &(*itemIt);
			modelRowIndex++;
		}
	}
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
	return static_cast<int>(m_groupRefs.size() + m_itemRefs.size());
}

QVariant ModelDataAccessorSummary::itemValue(const QModelIndex& index) const
{
	auto groupIt = m_groupRefs.find(index.row());
	if (groupIt != m_groupRefs.end())
	{
		return groupIt->second->groupName;
	}

	auto itemIt = m_itemRefs.find(index.row());
	assert(itemIt != m_itemRefs.end());
	return index.column() == 0 ? QVariant(itemIt->second->name) : QVariant(itemIt->second->issueCount);
}

QColor ModelDataAccessorSummary::itemBackgroundColor(const QModelIndex& index) const
{
	return QColor();
}

int ModelDataAccessorSummary::itemColSpan(const QModelIndex& index) const
{
	return m_groupRefs.find(index.row()) != m_groupRefs.end() ? 2 : 1;
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
	static QPixmap s_okPixmap = getPixmapIcon(StatusOK, 18);
	//static QPixmap s_warningPixmap = getPixmapIcon(StatusWarning, 18);
	//static QPixmap s_errorPixmap = getPixmapIcon(StatusError, 18);

	return !isGroupHeaderRow(index.row()) && index.column() == 0 ? &s_okPixmap : nullptr;
}

QObject* ModelDataAccessorSummary::qobject()
{
	return this;
}

std::vector<GridViewPainter*> ModelDataAccessorSummary::painters(const QModelIndex & index) const
{
	static GridViewPainterText s_textPainter;
	return { &s_textPainter };
}

bool ModelDataAccessorSummary::isGroupHeaderRow(int row) const
{
	return m_groupRefs.find(row) != m_groupRefs.end();
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

}