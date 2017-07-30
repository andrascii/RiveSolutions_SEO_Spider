#include "application.h"
#include "grid_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

GridModel::GridModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

int GridModel::rowCount(QModelIndex const&) const
{
	return m_accessor->itemCount();
}

int GridModel::columnCount(QModelIndex const&) const
{
	return m_accessor->columnsCount();
}

QVariant GridModel::data(QModelIndex const& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return m_accessor->item(index);
	}

	if (role == Qt::DecorationRole)
	{
// 		QPixmap* pixmap = m_accessor->pixmap(index);
// 
// 		if (pixmap != nullptr)
// 		{
// 			return *pixmap;
// 		}
	}

	if (role == Qt::BackgroundColorRole)
	{
		return backgroundColor();
	}

	if (role == Qt::TextColorRole)
	{
		return textColor();
	}

	if (role == Qt::UserRole)
	{
		//return m_accessor->flags(index);
	}

	return QVariant();
}

QVariant GridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical)
	{
		return section;
	}

	if (role == Qt::DisplayRole)
	{
		return m_accessor->columnDescription(section);
	}

	return QVariant();
}

void GridModel::setModelDataAccessor(std::unique_ptr<IGridDataAccessor> accessor)
{
	if (m_accessor)
	{
		VERIFY(disconnect(m_accessor->qobject(), SIGNAL(rowAdded(int)), 
			this, SLOT(onRowAdded(int))));

		VERIFY(disconnect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)),
			this, SLOT(onItemChanged(int, int))));
	}

	std::unique_ptr<IGridDataAccessor> oldAccessor = std::move(m_accessor);
	m_accessor = std::move(accessor);

	VERIFY(connect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	VERIFY(connect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)), this, SLOT(onItemChanged(int, int))));

	emit modelDataAccessorChanged(m_accessor.get(), oldAccessor.get());
	
	beginResetModel();
	endResetModel();
}

const IGridDataAccessor* GridModel::modelDataAcessor() const
{
	return m_accessor.get();
}

IGridDataAccessor* GridModel::modelDataAcessor()
{
	return m_accessor.get();
}

const IGridViewResizeStrategy* GridModel::resizeStrategy() const
{
	return m_accessor ? m_accessor->resizeStrategy() : nullptr;
}

IGridViewResizeStrategy* GridModel::resizeStrategy()
{
	return m_accessor ? m_accessor->resizeStrategy() : nullptr;
}

QColor GridModel::backgroundColor() const
{
	return m_backgroundColor;
}

void GridModel::setBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;

	Q_EMIT backgroundColorChanged(m_backgroundColor);
}

QColor GridModel::textColor() const
{
	return m_textColor;
}

Q_SLOT void GridModel::setTextColor(const QColor& color)
{
	m_textColor = color;

	Q_EMIT textColorChanged(m_textColor);
}

QColor GridModel::headerBackgroundColor() const
{
	return m_headerBackgroundColor;
}

void GridModel::setHeaderBackgroundColor(const QColor& color)
{
	m_headerBackgroundColor = color;

	Q_EMIT headerBackgroundColorChanged(m_headerBackgroundColor);
}

QColor GridModel::headerTextColor() const
{
	return m_headerTextColor;
}

void GridModel::setHeaderTextColor(const QColor& color)
{
	m_headerTextColor = color;

	Q_EMIT headerTextColorChanged(m_headerTextColor);
}

QFont GridModel::textFont() const
{
	return m_textFont;
}

Q_SLOT void GridModel::setTextFont(const QFont& font)
{
	if (m_textFont == font)
	{
		return;
	}

	m_textFont = font;

	Q_EMIT textFontChanged(m_textFont);
}

QColor GridModel::gridSelectionBackgroundColor() const
{
	return m_gridSelectionBackgroundColor;
}

Q_SLOT void GridModel::setGridSelectionBackgroundColor(const QColor& color)
{
	m_gridSelectionBackgroundColor = color;

	Q_EMIT gridSelectionBackgroundColorChanged(m_gridSelectionBackgroundColor);
}

QColor GridModel::gridSelectionBackgroundInactiveColor() const
{
	return m_gridSelectionBackgroundInactiveColor;
}

void GridModel::setGridSelectionBackgroundInactiveColor(const QColor& color)
{
	m_gridSelectionBackgroundInactiveColor = color;

	Q_EMIT gridSelectionBackgroundInactiveColorChanged(m_gridSelectionBackgroundInactiveColor);
}

QColor GridModel::gridSelectionBorderColor() const
{
	return m_gridSelectionBorderColor;
}

Q_SLOT void GridModel::setGridSelectionBorderColor(const QColor& color)
{
	m_gridSelectionBorderColor = color;

	Q_EMIT gridSelectionBorderColorChanged(m_gridSelectionBorderColor);
}

QColor GridModel::gridSelectionBorderInactiveColor() const
{
	return m_gridSelectionBorderInactiveColor;
}

void GridModel::setGridSelectionBorderInactiveColor(const QColor& color)
{
	m_gridSelectionBorderInactiveColor = color;

	Q_EMIT gridSelectionBorderColorInactiveChanged(m_gridSelectionBorderInactiveColor);
}

void GridModel::onRowAdded(int row)
{
	beginInsertRows(QModelIndex(), row, row);
	endInsertRows();
}

void GridModel::onItemChanged(int row, int column)
{
	QModelIndex idx = index(row, column);

	Q_EMIT dataChanged(idx, idx);
}

}