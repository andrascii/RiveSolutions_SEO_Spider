#include "grid_view.h"
#include "igrid_model.h"
#include "summary_model.h"
#include "igrid_view_resize_strategy.h"
#include "grid_view_selection_model.h"
#include "grid_view_delegate.h"
#include "context_menu_data_collection_row.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"
#include "grid_selection_background_item_painter.h"

namespace QuickieWebBot
{

GridView::GridView(QWidget * parent)
	: QTableView(parent)
	, m_gridViewModel(nullptr)
	, m_contextMenu(nullptr)
	, m_isCursorOverriden(false)
{
	setMouseTracking(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setItemDelegate(new GridViewDelegate(this));
	setSelectionModel(new GridViewSelectionModel(this));
}

void GridView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);

	assert(dynamic_cast<IGridModel*>(model));
	IGridModel* newModel = static_cast<IGridModel*>(model);

	if (newModel->resizeStrategy() != nullptr)
	{
		IGridViewResizeStrategy* oldStrategy = m_gridViewModel ? m_gridViewModel->resizeStrategy() : nullptr;
		newModel->resizeStrategy()->init(this, oldStrategy);
	}

	m_gridViewModel = newModel;
	
	initSpan();
}

void GridView::mouseMoveEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());

	if (index == m_hoveredIndex || !(index.flags() & Qt::ItemIsSelectable))
	{
		QTableView::mouseMoveEvent(event);
		return;
	}

	QAbstractItemModel* viewModel = model();
	int columnCount = viewModel->columnCount();

	m_hoveredIndex = index;

	QTableView::mouseMoveEvent(event);
}

void GridView::leaveEvent(QEvent* event)
{
	if (int hoveredIndexRow = m_hoveredIndex.row(); m_hoveredIndex.isValid())
	{
		QAbstractItemModel* viewModel = model();
		m_hoveredIndex = QModelIndex();
	}

	QTableView::leaveEvent(event);
}

void GridView::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);

	if (m_gridViewModel != nullptr && m_gridViewModel->resizeStrategy() != nullptr)
	{
		m_gridViewModel->resizeStrategy()->resize(this);
	}
}

void GridView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::RightButton || !m_contextMenu)
	{
		return QTableView::mouseReleaseEvent(event);
	}

	QPoint globalPosition = event->globalPos();

	selectRow(event->pos());

	m_contextMenu->move(globalPosition.x(), globalPosition.y());
	m_contextMenu->show();
}

QModelIndex GridView::hoveredIndex() const
{
	return m_hoveredIndex;
}

void GridView::setContextMenu(ContextMenuDataCollectionRow* menu)
{
	m_contextMenu = menu;
}

QList<IGridViewPainter*> GridView::painters() const noexcept
{
	static GridViewPainterText s_painterText;
	static GridViewPainterBackground s_backgroundPainter(Qt::transparent, Qt::transparent);
	static GridSelectionBackgroundItemPainter s_selectionBackgroundPainter(QColor(97, 160, 50, 255), QColor(97, 160, 50, 255));

	return QList<IGridViewPainter*>()
		<< &s_selectionBackgroundPainter
		<< &s_backgroundPainter
		<< &s_painterText;
}

void GridView::initSpan()
{
	int rows = model()->rowCount();
	int columns = model()->columnCount();

	for (int row = 0; row < rows; ++row)
	{
		for (int column = 0; column < columns;)
		{
			QSize colSpan = model()->span(model()->index(row, column));

			setSpan(row, column, colSpan.height(), colSpan.width());
			column += colSpan.width();
		}
	}
}

void GridView::selectRow(const QPoint& point)
{
	QModelIndex rowIndex = indexAt(point);
	QItemSelectionModel* modelSelection = selectionModel();

	QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::Rows;

	foreach(QModelIndex index, modelSelection->selectedRows())
	{
		if (index.row() == rowIndex.row())
		{
			flags |= QItemSelectionModel::Select;
		}
	}

	if (!(flags & QItemSelectionModel::Select))
	{
		flags |= QItemSelectionModel::ClearAndSelect;
	}

	modelSelection->select(rowIndex, flags);
}

}