#include "table_view.h"
#include "itable_model.h"
#include "iview_model.h"
#include "iresize_policy.h"
#include "selection_model.h"
#include "delegate.h"
#include "context_menu_data_collection_row.h"

namespace QuickieWebBot
{

TableView::TableView(QWidget * parent)
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

	setItemDelegate(new Delegate(this));
	setSelectionModel(new SelectionModel(this));
}

void TableView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);

	assert(dynamic_cast<ITableModel*>(model));
	m_model = static_cast<ITableModel*>(model);
	
	initSpan();
}

void TableView::mouseMoveEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());

	if (index == m_hoveredIndex || !(index.flags() & Qt::ItemIsSelectable))
	{
		QTableView::mouseMoveEvent(event);
		return;
	}

	m_hoveredIndex = index;

	QTableView::mouseMoveEvent(event);
}

void TableView::resizeEvent(QResizeEvent* event)
{
	if (m_viewModel && m_viewModel->resizePolicy())
	{
		m_viewModel->resizePolicy()->resize(this);
	}

	QTableView::resizeEvent(event);
}

void TableView::leaveEvent(QEvent* event)
{
	if (m_hoveredIndex.isValid())
	{
		m_hoveredIndex = QModelIndex();
	}

	QTableView::leaveEvent(event);
}

void TableView::contextMenuEvent(QContextMenuEvent* event)
{
	if (m_contextMenu)
	{
		m_contextMenu->exec(event->globalPos());
	}
}

QModelIndex TableView::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

void TableView::setContextMenu(ContextMenuDataCollectionRow* menu) noexcept
{
	m_contextMenu = menu;
}

void TableView::setViewModel(IViewModel* modelView) noexcept
{
	m_viewModel = modelView;

	if (m_viewModel && m_viewModel->resizePolicy())
	{
		IResizePolicy* prevPolicy = m_viewModel->resizePolicy();
		m_viewModel->resizePolicy()->init(this, prevPolicy);
	}
}

const IViewModel* TableView::viewModel() const noexcept
{
	return m_viewModel;
}

void TableView::initSpan()
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

void TableView::selectRow(const QPoint& point)
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

void TableView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QTableView::selectionChanged(selected, deselected);

	Q_EMIT selectionWasChanged(selected, deselected);
}

}