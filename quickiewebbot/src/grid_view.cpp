#include "grid_view.h"
#include "imodel_data_accessor.h"
#include "grid_view_model.h"
#include "igrid_view_resize_strategy.h"
#include "grid_view_selection_model.h"
#include "grid_view_delegate.h"


namespace QuickieWebBot
{

GridView::GridView(QWidget * parent)
	: QTableView(parent)
	, m_gridViewModel(nullptr)
	, m_isCursorOverriden(false)
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

	GridViewModel* newModel = dynamic_cast<GridViewModel*>(model);

	assert(newModel != nullptr);

	if (newModel->resizeStrategy() != nullptr)
	{
		IGridViewResizeStrategy* oldStrategy = m_gridViewModel != nullptr ? m_gridViewModel->resizeStrategy() : nullptr;
		newModel->resizeStrategy()->init(this, oldStrategy);
	}

	m_gridViewModel = newModel;

	VERIFY(QObject::connect(model, SIGNAL(modelDataAccessorChanged(IModelDataAccessor*, IModelDataAccessor*)), 
		this, SLOT(onModelDataAccessorChanged(IModelDataAccessor*, IModelDataAccessor*))));
	
	updateColumnsSpan();
	setSelectionModel(new GridViewSelectionModel(this));
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
	QAbstractItemModel* viewModel = model();
	int columnCount = viewModel->columnCount();

	if (m_hoveredIndex.isValid())
	{
		viewModel->dataChanged(viewModel->index(m_hoveredIndex.row(), 0), viewModel->index(m_hoveredIndex.row(), columnCount));
	}

	m_hoveredIndex = index;

	if (m_hoveredIndex.isValid())
	{
		viewModel->dataChanged(viewModel->index(m_hoveredIndex.row(), 0), viewModel->index(m_hoveredIndex.row(), columnCount));
	}

	QTableView::mouseMoveEvent(event);
}

void GridView::leaveEvent(QEvent* event)
{
	if (m_hoveredIndex.isValid())
	{
		QAbstractItemModel* viewModel = model();

		viewModel->dataChanged(viewModel->index(m_hoveredIndex.row(), 0), 
			viewModel->index(m_hoveredIndex.row(), viewModel->columnCount()));

		m_hoveredIndex = QModelIndex();
	}

	updateCursor(IModelDataAccessor::ItemFlagNone);
	
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
	if (dataAccessor())
	{
		ModelDataAccessorFactoryParams params = dataAccessor()->childViewParams(selected);
		emit childViewParamsChanged(params);
	}
	
	QTableView::selectionChanged(selected, deselected);
}

void GridView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() != Qt::RightButton)
	{
		return QTableView::mousePressEvent(event);
	}

	if (!m_rowMenu)
	{
		m_rowMenu = new QMenu(this);
		m_rowMenu->addAction(QIcon(), tr("Open Url"));
		m_rowMenu->addAction(QIcon(), tr("Copy"));
		m_rowMenu->addAction(QIcon(), tr("Refresh All Results"));
		m_rowMenu->addAction(QIcon(), tr("Remove Row"));
		m_rowMenu->addSeparator();
		m_rowMenu->addAction(QIcon(), tr("Check Index"));
		m_rowMenu->addAction(QIcon(), tr("Check Google Cache"));
		m_rowMenu->addAction(QIcon(), tr("Open robots.txt"));
		m_rowMenu->addAction(QIcon(), tr("Show Other Domains on IP"));
		m_rowMenu->addAction(QIcon(), tr("Check HTML With W3C Validator"));
	}

	QPoint globalPosition = event->globalPos();

	m_rowMenu->move(globalPosition.x(), globalPosition.y());
	m_rowMenu->show();
}

IModelDataAccessor* GridView::dataAccessor()
{
	return m_gridViewModel != nullptr ? m_gridViewModel->modelDataAcessor() : nullptr;
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
	if (m_gridViewModel == nullptr)
	{
		GridViewModel* model = new GridViewModel(this);
		model->setModelDataAccessor(factory.getModelDataAccessor(params));

		setModel(model);

		return;
	}

	m_gridViewModel->setModelDataAccessor(factory.getModelDataAccessor(params));
}

void GridView::paintEvent(QPaintEvent* event)
{
	QTableView::paintEvent(event);
}

void GridView::onModelDataAccessorChanged(IModelDataAccessor* accessor, IModelDataAccessor* oldAccessor)
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
			int colSpan = dataAccessor()->itemColSpan(model()->index(row, column));

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