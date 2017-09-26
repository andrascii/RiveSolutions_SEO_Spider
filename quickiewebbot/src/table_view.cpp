#include "table_view.h"
#include "abstract_table_model.h"
#include "iview_model.h"
#include "iresize_policy.h"
#include "selection_model.h"
#include "item_view_delegate.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TableView::TableView(QWidget* parent)
	: QTableView(parent)
	, m_model(nullptr)
	, m_viewModel(nullptr)
	, m_contextMenu(nullptr)
	, m_showAdditionalGrid(false)
	, m_rowHeight(QuickieWebBotHelpers::pointsToPixels(25))
{
	setMouseTracking(true);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

	setItemDelegate(new ItemViewDelegate(nullptr, this));
	setSelectionModel(new SelectionModel(this));

	horizontalHeader()->setSectionsMovable(true);

	//
	// Use GridLineRenderer in a view model for this
	//
	setShowGrid(false);
}

void TableView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);

	m_model = QuickieWebBotHelpers::safe_runtime_static_cast<AbstractTableModel*>(model);

	for (int i = 0; i < m_model->rowCount(); ++i)
	{
		setRowHeight(i, m_rowHeight);
	}

	if (m_model->resizePolicy())
	{
		IResizePolicy* prevPolicy = m_model->resizePolicy();
		m_model->resizePolicy()->init(this, prevPolicy);

		VERIFY(connect(m_model->resizePolicy()->qobject(), SIGNAL(columnSizeChanged()), this, SLOT(adjustColumnSize())));
	}
	
	initSpans();
	setSelectionModel(new SelectionModel(this));

	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(initSpans())));
	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(adjustColumnSize())));
}

void TableView::mouseMoveEvent(QMouseEvent* event)
{
	if (!viewModel())
	{
		return QTableView::mouseMoveEvent(event);
	}

	QModelIndex index = indexAt(event->pos());

	if (!(index.flags() & Qt::ItemIsSelectable))
	{
		viewModel()->setHoveredIndex(QModelIndex());
	}
	else
	{
		viewModel()->setHoveredIndex(index);
	}

	QTableView::mouseMoveEvent(event);
}

void TableView::resizeEvent(QResizeEvent* event)
{
	if (m_model && m_model->resizePolicy())
	{
		m_model->resizePolicy()->resize(this);
	}

	QTableView::resizeEvent(event);
}

void TableView::leaveEvent(QEvent* event)
{
	if (viewModel() && viewModel()->hoveredIndex().isValid())
	{
		viewModel()->setHoveredIndex(QModelIndex());
	}

	QTableView::leaveEvent(event);
}

void TableView::contextMenuEvent(QContextMenuEvent* event)
{
	if (!m_contextMenu)
	{
		return;
	}

	m_contextMenu->exec(event->globalPos());
}

void TableView::paintEvent(QPaintEvent* event)
{
	QTableView::paintEvent(event);

	if (!showAdditionalGrid() || !viewModel())
	{
		return;
	}

	struct VerticalLineSegment
	{
		int firstYCoordinate;
		int secondYCoordinate;
	};

	auto makeUniqueSelectedRows = [this]() -> std::vector<int>
	{
		std::vector<int> uniqueSelectedRows;

		foreach(const QModelIndex& index, selectedIndexes())
		{
			uniqueSelectedRows.push_back(index.row());
		}

		std::sort(std::begin(uniqueSelectedRows), std::end(uniqueSelectedRows));
		uniqueSelectedRows.erase(std::unique(std::begin(uniqueSelectedRows), std::end(uniqueSelectedRows)), std::end(uniqueSelectedRows));

		return uniqueSelectedRows;
	};

	QPainter painter(viewport());

	const QRect viewportRect = viewport()->rect();
	const int pseudoRowCount = viewportRect.height() / m_rowHeight + 1;

	std::vector<int> uniqueSelectedRows = makeUniqueSelectedRows();

	//
	// draw horizontal row grid lines
	//

	for (int i = 0; i < pseudoRowCount; ++i)
	{
		const int offsetByY = m_rowHeight * i - 1;

		const int thisRowLogicalIndex = rowAt(offsetByY);
		const int nextRowLogicalIndex = rowAt(offsetByY + m_rowHeight);

		const bool isThisOrNextRowSelected = std::find_if(
			std::begin(uniqueSelectedRows), 
			std::end(uniqueSelectedRows), 
			[&](int selectedRow) { return selectedRow == thisRowLogicalIndex || selectedRow == nextRowLogicalIndex; }) != 
			std::end(uniqueSelectedRows);

		if (isThisOrNextRowSelected)
		{
			painter.save();

			painter.setPen(viewModel()->selectedGridLineColor(QModelIndex()));
			painter.drawLine(QPoint(0, offsetByY), QPoint(width(), offsetByY));

			painter.restore();
		}
		else
		{
			painter.drawLine(QPoint(0, offsetByY), QPoint(width(), offsetByY));
		}
	}

	//
	// draw vertical column grid lines
	//

	std::vector<VerticalLineSegment> verticalLineSegments;

	for (int i = 0, size = static_cast<int>(uniqueSelectedRows.size()); i < size; ++i)
	{
		const int topRowLineYCoordinate = m_rowHeight * uniqueSelectedRows[i];

		verticalLineSegments.push_back(VerticalLineSegment{ topRowLineYCoordinate, topRowLineYCoordinate + m_rowHeight });
	}

	for (int i = 0; i < horizontalHeader()->count(); ++i)
	{
		const int offsetByX = 
			horizontalHeader()->sectionViewportPosition(i) + 
			horizontalHeader()->sectionSize(i) - 1;

		painter.drawLine(QPoint(offsetByX, 0), QPoint(offsetByX, viewportRect.height()));

		painter.save();
		painter.setPen(viewModel()->selectedGridLineColor(QModelIndex()));

		for (int j = 0, size = static_cast<int>(verticalLineSegments.size()); j < size; ++j)
		{
			painter.drawLine(QPoint(offsetByX, verticalLineSegments[j].firstYCoordinate), 
				QPoint(offsetByX, verticalLineSegments[j].secondYCoordinate));
		}

		painter.restore();
	}
}

void TableView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (viewModel())
	{
		viewModel()->setDeselectedIndexes(deselected.indexes());
		viewModel()->setSelectedIndexes(selected.indexes());
	}

	QTableView::selectionChanged(selected, deselected);
}

void TableView::rowsInserted(const QModelIndex &parent, int start, int end)
{
	for (int i = start; i <= end; ++i)
	{
		setRowHeight(i, m_rowHeight);
	}

	QTableView::rowsInserted(parent, start, end);
}

void TableView::setContextMenu(QMenu* menu) noexcept
{
	m_contextMenu = menu;
}

void TableView::setViewModel(IViewModel* modelView) noexcept
{
	if (m_viewModel)
	{
		disconnect(m_viewModel->qobject(), SIGNAL(repaintItems(const QModelIndexList&)), 
			this, SLOT(onAboutRepaintItems(const QModelIndexList&)));
	}

	m_viewModel = modelView;

	VERIFY(connect(m_viewModel->qobject(), SIGNAL(repaintItems(const QModelIndexList&)),
		this, SLOT(onAboutRepaintItems(const QModelIndexList&))));

	QuickieWebBotHelpers::safe_runtime_static_cast<ItemViewDelegate*>(itemDelegate())->setViewModel(m_viewModel);
}

IViewModel* TableView::viewModel() const noexcept
{
	return m_viewModel;
}

void TableView::setShowAdditionalGrid(bool value) noexcept
{
	m_showAdditionalGrid = value;
}

bool TableView::showAdditionalGrid() const noexcept
{
	return m_showAdditionalGrid;
}

void TableView::initSpans()
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

void TableView::adjustColumnSize()
{
	m_model = QuickieWebBotHelpers::safe_runtime_static_cast<AbstractTableModel*>(model());

	if (m_model->resizePolicy())
	{
		IResizePolicy* prevPolicy = m_model->resizePolicy();
		m_model->resizePolicy()->init(this, prevPolicy);
	}
}

void TableView::onAboutRepaintItems(const QModelIndexList& modelIndexes)
{
	if (!isVisible())
	{
		return;
	}

	foreach(const QModelIndex& index, modelIndexes)
	{
		if (!index.isValid())
		{
			continue;
		}

		update(index);
	}
}

}