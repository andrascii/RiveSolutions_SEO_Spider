#include "summary_model.h"
#include "model_controller.h"
#include "data_collection.h"
#include "quickie_web_bot_helpers.h"
#include "grid_view_full_size_resize_policy.h"

namespace QuickieWebBot
{

SummaryModel::SummaryModel(QObject* parent)
	: IGridModel(parent)
	, m_resizeStrategy(std::make_unique<GridViewFullSizeResizePolicy>(std::vector<int>{ 60, 40 }))
{
	using namespace WebCrawler;
	m_allGroups =
	{
		SummaryGroup
		{
			"Links",
			{
				SummaryItem{ "Links with non-ASCII characters", DataCollection::NonAsciiCharacterUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Links with uppercase characters", DataCollection::UpperCaseUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too long links", DataCollection::VeryLongUrlStorageType, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Title",
			{
				SummaryItem{ "Empty titles", DataCollection::EmptyTitleUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Duplicate titles", DataCollection::DuplicatedTitleUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too long titles", DataCollection::VeryLongTitleUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too short titles", DataCollection::VeryShortTitleUrlStorageType, StatusOK, 0 },
				SummaryItem{ "H1 Duplcates titles", DataCollection::DuplicatedH1TitleUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Several title tags", DataCollection::SeveralTitleUrlStorageType, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Description",
			{
				SummaryItem{ "Empty meta description", DataCollection::EmptyMetaDescriptionUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Duplicate meta descriptions", DataCollection::DuplicatedMetaDescriptionUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too long meta descriptions", DataCollection::VeryLongMetaDescriptionUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too short meta descriptions", DataCollection::VeryShortMetaDescriptionUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Several meta descriptions tags", DataCollection::SeveralMetaDescriptionUrlStorageType, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Meta Key Words",
			{
				SummaryItem{ "Empty meta keywords", DataCollection::EmptyMetaKeywordsUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Duplicate meta keywords", DataCollection::DuplicatedMetaKeywordsUrlStorageType, StatusOK, 0 },
				SummaryItem{ "Several meta keywords tags", DataCollection::SeveralMetaKeywordsUrlStorageType, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H1 Headers",
			{
				SummaryItem{ "Missing H1", DataCollection::MissingH1UrlStorageType, StatusOK, 0 },
				SummaryItem{ "Duplicate H1 headers", DataCollection::DuplicatedH1UrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too long H1 headers", DataCollection::VeryLongH1UrlStorageType, StatusOK, 0 },
				SummaryItem{ "Several H1 tags", DataCollection::SeveralH1UrlStorageType, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's H2 Headers",
			{
				SummaryItem{ "Missing H2", DataCollection::MissingH2UrlStorageType, StatusOK, 0 },
				SummaryItem{ "Duplicate H2 headers", DataCollection::DuplicatedH2UrlStorageType, StatusOK, 0 },
				SummaryItem{ "Too long H2 headers", DataCollection::VeryLongH2UrlStorageType, StatusOK, 0 },
				SummaryItem{ "Several H2 tags", DataCollection::SeveralH2UrlStorageType, StatusOK, 0 },
			}
		},
		SummaryGroup
		{
			"Page's Images",
			{
				SummaryItem{ "Over 100KB size", DataCollection::Over100kbImageStorageType, StatusOK, 0 },
				SummaryItem{ "Missing alt description", DataCollection::MissingAltTextImageStorageType, StatusOK, 0 },
				SummaryItem{ "Too long alt descriptions", DataCollection::VeryLongAltTextImageStorageType, StatusOK, 0 },
			}
		}
	};

	int modelRowIndex = 0;

	for (auto groupIt = std::begin(m_allGroups); groupIt != std::end(m_allGroups); ++groupIt)
	{
		m_groups[modelRowIndex] = &(*groupIt);
		modelRowIndex++;

		for (auto itemIt = std::begin(groupIt->groupItems); itemIt != std::end(groupIt->groupItems); ++itemIt)
		{
			m_itemRows[modelRowIndex] = &(*itemIt);
			m_itemTypes[itemIt->id] = &(*itemIt);
			itemIt->row = modelRowIndex;
			modelRowIndex++;
		}
	}
}

Qt::ItemFlags SummaryModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled;

	if (!isGroupHeaderRow(index.row()))
	{
		flags |= Qt::ItemIsSelectable;
	}

	return flags;
}

QSize SummaryModel::span(const QModelIndex& index) const
{
	return m_groups.find(index.row()) != m_groups.end() ? QSize(2, 1) : QSize(1, 1);
}

QVariant SummaryModel::data(const QModelIndex& index, int role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
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

		case Qt::DecorationRole:
		{
			static QPixmap s_okPixmap = pixmap(StatusOK, QuickieWebBotHelpers::pointsToPixels(13.5));
			static QPixmap s_warningPixmap = pixmap(StatusWarning, QuickieWebBotHelpers::pointsToPixels(13.5));
			static QPixmap s_errorPixmap = pixmap(StatusError, QuickieWebBotHelpers::pointsToPixels(13.5));

			if (isGroupHeaderRow(index.row()) || index.column() != 0)
			{
				return QVariant();
			}

			auto it = m_itemRows.find(index.row());
			assert(it != m_itemRows.end());

			return it.value()->issueCount > 0 ? s_warningPixmap : s_okPixmap;
		}

		case Qt::FontRole:
		{
			QFont font;

			if (isGroupHeaderRow(index.row()))
			{
				font.setBold(true);
			}

			return font;
		}

		case Qt::TextAlignmentRole:
		{
			if (index.column() != 0)
			{
				return Qt::AlignRight;
			}

			return Qt::AlignLeft;
		}

		case Qt::BackgroundColorRole:
		{
			return QColor(Qt::transparent);
		}

		case IGridModel::SelectionBackgroundColorRole:
		{
			return QColor(7, 160, 50, 255);
		}

		case IGridModel::MarginTop:
		{
			return QuickieWebBotHelpers::pointsToPixels(2);
		}

		case IGridModel::MarginBottom:
		{
			return QuickieWebBotHelpers::pointsToPixels(2);
		}

		case IGridModel::MarginLeft:
		{
			return QuickieWebBotHelpers::pointsToPixels(4);
		}

		case IGridModel::MarginRight:
		{
			return QuickieWebBotHelpers::pointsToPixels(4);
		}
	}

	return QVariant();
}

QVariant SummaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

int SummaryModel::columnCount(const QModelIndex& parent) const
{
	return s_summaryColumnCount;
}

int SummaryModel::rowCount(const QModelIndex& parent) const
{
	return m_groups.size() + m_itemRows.size();
}

bool SummaryModel::isGroupHeaderRow(int row) const
{
	return m_groups.find(row) != m_groups.end();
}

QPixmap SummaryModel::pixmap(ItemStatus status, int size) const
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