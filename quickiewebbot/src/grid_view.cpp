#include "grid_view.h"
#include "igrid_model.h"
#include "igrid_view_model.h"
#include "igrid_view_resize_strategy.h"
#include "grid_view_selection_model.h"
#include "grid_view_delegate.h"
#include "context_menu_data_collection_row.h"

namespace QuickieWebBot
{

GridView::GridView(QWidget * parent)
	: QTableView(parent)
	, m_model(nullptr)
	, m_viewModel(nullptr)
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

	m_model = static_cast<IGridModel*>(model);
	
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
	if (m_viewModel && m_viewModel->resizeStrategy())
	{
		m_viewModel->resizeStrategy()->resize(this);
	}

	QTableView::resizeEvent(event);
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

void GridView::setViewModel(IGridViewModel* modelView)
{
	m_viewModel = modelView;

	if (m_viewModel->resizeStrategy())
	{
		IGridViewResizePolicy* prevPolicy = m_viewModel ? m_viewModel->resizeStrategy() : nullptr;
		m_viewModel->resizeStrategy()->init(this, prevPolicy);
	}
}

const IGridViewModel* GridView::viewModel() const noexcept
{
	return m_viewModel;
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