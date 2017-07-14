#include "grid_view.h"
#include "imodel_data_accessor.h"
#include "grid_view_model.h"


namespace QuickieWebBot
{

GridView::GridView(QWidget * parent)
	: QTableView(parent)
	, m_accessor(nullptr)
{
	setMouseTracking(true);
}

void GridView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);

	GridViewModel* gridViewModel = dynamic_cast<GridViewModel*>(model);
	assert(gridViewModel != nullptr);
	m_accessor = gridViewModel->dataAcessor();

	VERIFY(QObject::connect(model, SIGNAL(modelAccessorChanged(IModelDataAccessor*)), this, SLOT(onModelAccessorChanged(IModelDataAccessor*))));
}

void GridView::mouseMoveEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	if (index != m_hoveredIndex)
	{
		m_hoveredIndex = index;
		// raise paint event on prev and current hovered row
	}

	QTableView::mouseMoveEvent(event);
}

IModelDataAccessor* GridView::dataAccessor()
{
	return m_accessor;
}

QModelIndex GridView::hoveredIndex() const
{
	return m_hoveredIndex;
}

void GridView::paintEvent(QPaintEvent* event)
{
	QTableView::paintEvent(event);

}

void GridView::onModelAccessorChanged(IModelDataAccessor* accessor)
{
	m_accessor = accessor;
}

}