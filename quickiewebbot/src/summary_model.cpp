#include "summary_model.h"
#include "model_controller.h"
#include "data_collection.h"
#include "quickie_web_bot_helpers.h"
#include "viewport_percent_resize_policy.h"

namespace QuickieWebBot
{

SummaryModel::SummaryModel(QObject* parent)
	: ITableModel(parent)
	, m_resizePolicy(std::make_shared<ViewportPercentResizePolicy>(std::vector<int>{ 85, 15 }))
	, m_dataAccessor(nullptr)
{
}

Qt::ItemFlags SummaryModel::flags(const QModelIndex& index) const
{
	return dataAccessor() ? dataAccessor()->flags(index) : Qt::NoItemFlags;
}

IResizePolicy* SummaryModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}


void SummaryModel::setDataAccessor(ISummaryDataAccessor* dataAccessor) noexcept
{
	m_dataAccessor = dataAccessor;
}

ISummaryDataAccessor* SummaryModel::dataAccessor() const noexcept
{
	return m_dataAccessor;
}

SummaryCategoryItem SummaryModel::itemCategory(const QModelIndex& index) const noexcept
{
	if (!dataAccessor())
	{
		return SummaryCategoryItem::SummaryCategoryItemHeader;
	}

	return dataAccessor()->itemCategory(index);
}

QSize SummaryModel::span(const QModelIndex& index) const
{
	if (!dataAccessor())
	{
		return QAbstractTableModel::span(index);
	}

	return dataAccessor()->span(index);
}

QVariant SummaryModel::data(const QModelIndex& index, int role) const
{
	if (!dataAccessor())
	{
		return QVariant();
	}

	switch (role)
	{
		case Qt::AccessibleTextRole:
		case Qt::AccessibleDescriptionRole:
		case Qt::DisplayRole:
		{
			return dataAccessor()->item(index);
		}

		case Qt::DecorationRole:
		{
			return dataAccessor()->pixmap(index);
		}

		case Qt::FontRole:
		{
			QFont font = Application::font();

			if (!dataAccessor())
			{
				return font;
			}

			if (dataAccessor()->isHeaderItem(index))
			{
				font.setBold(true);
				return font;
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

		case ITableModel::SelectionBackgroundColorRole:
		{
			return QColor(97, 160, 50, 150);
		}

		case ITableModel::MarginTop:
		case ITableModel::MarginBottom:
		{
			return QuickieWebBotHelpers::pointsToPixels(4);
		}

		case ITableModel::MarginRight:
		case ITableModel::MarginLeft:
		{
			return QuickieWebBotHelpers::pointsToPixels(2);
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
	if (!dataAccessor())
	{
		return 0;
	}

	return dataAccessor()->columnCount();
}

int SummaryModel::rowCount(const QModelIndex& parent) const
{
	if (!dataAccessor())
	{
		return 0;
	}

	return dataAccessor()->rowCount();
}

}