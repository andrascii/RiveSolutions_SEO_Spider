#include "summary_filter_data_accessor.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

SummaryFilterDataAccessor::SummaryFilterDataAccessor()
{
	using namespace WebCrawler;

	m_allGroups =
	{
		SummaryGroup
		{
			"Links",
			{
				SummaryItem{ "Links with non-ASCII characters", SummaryCategoryItem::SummaryCategoryItemNonAsciiLinks, StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", SummaryCategoryItem::SummaryCategoryItemUpperCaseLinks, StatusOK, 0 },
				SummaryItem{ "Too long links", SummaryCategoryItem::SummaryCategoryItemVeryLongLinks, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", SummaryCategoryItem::SummaryCategoryItemEmptyTitles, StatusOK, 0 },
				SummaryItem{ "Duplicate titles", SummaryCategoryItem::SummaryCategoryItemDuplicatedTitles, StatusOK, 0 },
				SummaryItem{ "Too long titles", SummaryCategoryItem::SummaryCategoryItemVeryLongTitles, StatusOK, 0 },
				SummaryItem{ "Too short titles", SummaryCategoryItem::SummaryCategoryItemVeryShortTitles, StatusOK, 0 },
				SummaryItem{ "H1 Duplicates titles", SummaryCategoryItem::SummaryCategoryItemDuplicatedH1Titles, StatusOK, 0 },
				SummaryItem{ "Several title tags", SummaryCategoryItem::SummaryCategoryItemSeveralTitlesOnPage, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", SummaryCategoryItem::SummaryCategoryItemEmptyMetaDescriptions, StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", SummaryCategoryItem::SummaryCategoryItemDuplicatedMetaDescriptions, StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", SummaryCategoryItem::SummaryCategoryItemVeryLongMetaDescriptions, StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", SummaryCategoryItem::SummaryCategoryItemVeryShortMetaDescriptions, StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", SummaryCategoryItem::SummaryCategoryItemSeveralMetaDescriptionsOnPage, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta keywords", SummaryCategoryItem::SummaryCategoryItemEmptyMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Duplicate meta keywords", SummaryCategoryItem::SummaryCategoryItemDuplicatedMetaKeywords, StatusOK, 0 },
				SummaryItem{ "Several meta keywords tags", SummaryCategoryItem::SummaryCategoryItemSeveralMetaKeywordsOnPage, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", SummaryCategoryItem::SummaryCategoryItemMissingH1s, StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", SummaryCategoryItem::SummaryCategoryItemDuplicatedH1s, StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", SummaryCategoryItem::SummaryCategoryItemVeryLongH1s, StatusOK, 0 },
				SummaryItem{ "Several H1 tags", SummaryCategoryItem::SummaryCategoryItemSeveralH1s, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", SummaryCategoryItem::SummaryCategoryItemMissingH2s, StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", SummaryCategoryItem::SummaryCategoryItemDuplicatedH2s, StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", SummaryCategoryItem::SummaryCategoryItemVeryLongH2s, StatusOK, 0 },
				SummaryItem{ "Several H2 tags", SummaryCategoryItem::SummaryCategoryItemSeveralH2s, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", SummaryCategoryItem::SummaryCategoryItemImagesOver100kb, StatusOK, 0 },
				SummaryItem{ "Missing alt description", SummaryCategoryItem::SummaryCategoryItemImageMissingAltText, StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", SummaryCategoryItem::SummaryCategoryItemImagesVeryLongAltText, StatusOK, 0 },
			}
		}
	};

	int modelRowIndex = 0;

	for (auto start = std::begin(m_allGroups); start != std::end(m_allGroups); ++start)
	{
		m_groups[modelRowIndex] = &(*start);
		++modelRowIndex;

		for (auto itemStart = std::begin(start->groupItems); itemStart != std::end(start->groupItems); ++itemStart)
		{
			m_itemRows[modelRowIndex] = &(*itemStart);
			m_itemTypes[itemStart->category] = &(*itemStart);
			itemStart->row = modelRowIndex;
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
	return m_groups.size() + m_itemRows.size();
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
	return m_groups.find(index.row()) != m_groups.end() ? QSize(2, 1) : QSize(1, 1);
}

QVariant SummaryFilterDataAccessor::item(const QModelIndex& index) const noexcept
{
	auto groupIterator = m_groups.find(index.row());

	if (groupIterator != m_groups.end())
	{
		return groupIterator.value()->groupName;
	}

	auto itemIterator = m_itemRows.find(index.row());

	assert(itemIterator != m_itemRows.end());

	return index.column() == 0 ? QVariant(itemIterator.value()->name) : QVariant(itemIterator.value()->issueCount);
}

bool SummaryFilterDataAccessor::isHeaderItem(const QModelIndex& index) const noexcept
{
	return m_groups.find(index.row()) != m_groups.end();
}

SummaryCategoryItem SummaryFilterDataAccessor::itemCategory(const QModelIndex& index) const noexcept
{
	if (isHeaderItem(index))
	{
		return SummaryCategoryItem::SummaryCategoryItemNone;
	}

	return m_itemRows[index.row()]->category;
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

	QPixmap pixmap(QSize(QuickieWebBotHelpers::pointsToPixels(13.5), QuickieWebBotHelpers::pointsToPixels(13.5)));
	pixmap.fill(Qt::transparent);

	QPainter painterPixmap(&pixmap);
	renderer.render(&painterPixmap);

	return pixmap;
}

SummaryFilterDataAccessor::ItemStatus SummaryFilterDataAccessor::itemStatus(const QModelIndex& index) const noexcept
{
	return m_itemRows[index.row()]->status;
}

}