#include "model_data_accessor_summary.h"
#include "grid_view_painter_text.h"

namespace QuickieWebBot
{

ModelDataAccessorSummary::ModelDataAccessorSummary()
{
	m_groups.push_back(SummaryGroup 
	{
		"Indexing and crawlability",
		{
			SummaryItem{ "Resources with 4xx status code", "4xx_items", StatusOK, 0 },
			SummaryItem{ "Resources with 5xx status code", "5xx_items", StatusOK, 0 },
			SummaryItem{ "Resources restricted from indexing", "restricted_from_indexing", StatusOK, 0 },
		}
		
	});

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
	// just for test, will be implemented later
	static QPixmap s_pixmap = QPixmap(":/images/arrow-down-icon.png");
	return !isGroupHeaderRow(index.row()) && index.column() == 0 ? &s_pixmap : nullptr;
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

}