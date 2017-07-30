#include "grid_view.h"
#include "igrid_data_accessor.h"
#include "grid_model.h"
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
	, m_isCursorOverriden(false)
	, m_contextMenu(nullptr)
{
	setMouseTracking(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

	setItemDelegate(new GridViewDelegate(this));
}

void GridView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);

	assert(dynamic_cast<GridModel*>(model));
	GridModel* newModel = static_cast<GridModel*>(model);

	if (newModel->resizeStrategy() != nullptr)
	{
		IGridViewResizeStrategy* oldStrategy = m_gridViewModel != nullptr ? m_gridViewModel->resizeStrategy() : nullptr;
		newModel->resizeStrategy()->init(this, oldStrategy);
	}

	m_gridViewModel = newModel;

	VERIFY(connect(model, SIGNAL(modelDataAccessorChanged(IGridDataAccessor*, IGridDataAccessor*)), 
		this, SLOT(onModelDataAccessorChanged(IGridDataAccessor*, IGridDataAccessor*))));
	
	updateColumnsSpan();
	setSelectionModel(new GridViewSelectionModel(this));
}

void GridView::mouseMoveEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	int flags = index.data(Qt::UserRole).toInt();

	updateCursor(flags);

	if (index == m_hoveredIndex || !(index.flags() & Qt::ItemIsSelectable))
	{
		QTableView::mouseMoveEvent(event);
		return;
	}

	QAbstractItemModel* viewModel = model();
	int columnCount = viewModel->columnCount();

	m_hoveredIndex = index;

	if (int hoveredIndexRow = m_hoveredIndex.row(); m_hoveredIndex.isValid())
	{
		emit viewModel->dataChanged(viewModel->index(hoveredIndexRow, 0), viewModel->index(hoveredIndexRow, columnCount));
	}

	QTableView::mouseMoveEvent(event);
}

void GridView::leaveEvent(QEvent* event)
{
	if (int hoveredIndexRow = m_hoveredIndex.row(); m_hoveredIndex.isValid())
	{
		QAbstractItemModel* viewModel = model();

		emit viewModel->dataChanged(viewModel->index(hoveredIndexRow, 0), viewModel->index(hoveredIndexRow, viewModel->columnCount()));

		m_hoveredIndex = QModelIndex();
	}

	//updateCursor(IModelDataAccessor::ItemFlagNone);
	
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

void GridView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (modelDataAccessor())
	{
		GridDataAccessorFactoryParams params = modelDataAccessor()->childViewParams(selected);
		emit childViewParamsChanged(params);
	}
	
	QTableView::selectionChanged(selected, deselected);
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

IGridDataAccessor* GridView::modelDataAccessor()
{
	return m_gridViewModel != nullptr ? m_gridViewModel->modelDataAcessor() : nullptr;
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

void GridView::setParams(const GridDataAccessorFactoryParams& params)
{
	if (!params.isValid())
	{
		return;
	}

	GridDataAccessorFactory factory;

	if (!m_gridViewModel)
	{
		GridModel* model = new GridModel(this);
		model->setModelDataAccessor(factory.create(params));

		setModel(model);
		return;
	}

	m_gridViewModel->setModelDataAccessor(factory.create(params));
}

void GridView::onModelDataAccessorChanged(IGridDataAccessor* accessor, IGridDataAccessor* oldAccessor)
{
	if (accessor->resizeStrategy() != nullptr)
	{
		IGridViewResizeStrategy* oldStrategy = oldAccessor != nullptr ? oldAccessor->resizeStrategy() : nullptr;
		accessor->resizeStrategy()->init(this, oldStrategy);
	}

	updateColumnsSpan();
}

void GridView::updateColumnsSpan()
{
	int rows = model()->rowCount();
	int columns = model()->columnCount();

	for (int row = 0; row < rows; ++row)
	{
		for (int column = 0; column < columns;)
		{
			int colSpan = 1;// modelDataAccessor()->itemColSpan(model()->index(row, column));

			assert(colSpan > 0);

			setSpan(row, column, 1, colSpan);
			column += colSpan;
		}
	}
}

void GridView::updateCursor(int flags)
{
// 	if (flags & IModelDataAccessor::ItemFlagUrl)
// 	{
// 		if (!m_isCursorOverriden)
// 		{
// 			QApplication::setOverrideCursor(Qt::PointingHandCursor);
// 			m_isCursorOverriden = true;
// 		}
// 	}
// 	else if (m_isCursorOverriden)
// 	{
// 		QApplication::restoreOverrideCursor();
// 		m_isCursorOverriden = false;
// 	}
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