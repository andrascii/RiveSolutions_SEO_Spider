#include "grid_view.h"
#include "imodel_data_accessor.h"
#include "grid_view_model.h"
#include "igrid_view_resize_strategy.h"
#include "grid_view_selection_model.h"


namespace QuickieWebBot
{

GridView::GridView(QWidget * parent)
	: QTableView(parent)
	, m_gridViewModel(nullptr)
	, m_accessor(nullptr)
	, m_isCursorOverriden(false)
{
	setMouseTracking(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void GridView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);

	GridViewModel* gridViewModel = dynamic_cast<GridViewModel*>(model);
	assert(gridViewModel != nullptr);
	m_accessor = gridViewModel->modelDataAcessor();
	m_gridViewModel = gridViewModel;

	VERIFY(QObject::connect(model, SIGNAL(modelDataAccessorChanged(IModelDataAccessor*)), this, SLOT(onModelDataAccessorChanged(IModelDataAccessor*))));
	updateColumnsSpan();
	setSelectionModel(new GridViewSelectionModel(this));
}

void GridView::setColumnResizeStrategy(std::unique_ptr<IGridViewResizeStrategy> strategy)
{
	m_resizeStrategy = std::move(strategy);
}

void GridView::mouseMoveEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	int flags = index.data(Qt::UserRole).toInt();
	updateCursor(flags);

	if (index == m_hoveredIndex || flags & IModelDataAccessor::ItemFlagNotSelectable)
	{
		QTableView::mouseMoveEvent(event);
		return;
	}

	// TODO: paint only selection
	QAbstractItemModel* mod = model();
	int columnCount = mod->columnCount();

	if (m_hoveredIndex.isValid())
	{
		mod->dataChanged(mod->index(m_hoveredIndex.row(), 0), mod->index(m_hoveredIndex.row(), columnCount));
	}

	m_hoveredIndex = index;

	if (m_hoveredIndex.isValid())
	{
		mod->dataChanged(mod->index(m_hoveredIndex.row(), 0), mod->index(m_hoveredIndex.row(), columnCount));
	}

	QTableView::mouseMoveEvent(event);
}

void GridView::leaveEvent(QEvent* event)
{
	if (m_hoveredIndex.isValid())
	{
		QAbstractItemModel* mod = model();
		mod->dataChanged(mod->index(m_hoveredIndex.row(), 0), mod->index(m_hoveredIndex.row(), mod->columnCount()));
		m_hoveredIndex = QModelIndex();
	}

	updateCursor(IModelDataAccessor::ItemFlagNone);
	
	QTableView::leaveEvent(event);
}

void GridView::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);

	if (m_resizeStrategy)
	{
		m_resizeStrategy->resize(this);
	}
}

void GridView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);
	if (m_accessor) 
	{
		ModelDataAccessorFactoryParams params = m_accessor->childViewParams(selected);
		emit childViewParamsChanged(params);
	}
	
	QTableView::selectionChanged(selected, deselected);
}

IModelDataAccessor* GridView::dataAccessor()
{
	return m_accessor;
}

QModelIndex GridView::hoveredIndex() const
{
	return m_hoveredIndex;
}

void GridView::setParams(const ModelDataAccessorFactoryParams& params)
{
	if (!params.isValid())
	{
		return;
	}

	ModelDataAccessorFactory factory;
	m_gridViewModel->setModelDataAccessor(factory.getModelDataAccessor(params));
}

void GridView::paintEvent(QPaintEvent* event)
{
	QTableView::paintEvent(event);

}

void GridView::onModelDataAccessorChanged(IModelDataAccessor* accessor)
{
	m_accessor = accessor;
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
			int colSpan = m_accessor->itemColSpan(model()->index(row, column));
			assert(colSpan > 0);
			setSpan(row, column, 1, colSpan);
			column += colSpan;
		}
	}
}

void GridView::updateCursor(int flags)
{
	if (flags & IModelDataAccessor::ItemFlagUrl)
	{
		if (!m_isCursorOverriden)
		{
			QApplication::setOverrideCursor(Qt::PointingHandCursor);
			m_isCursorOverriden = true;
		}
	}
	else if (m_isCursorOverriden)
	{
		QApplication::restoreOverrideCursor();
		m_isCursorOverriden = false;
	}
}

}