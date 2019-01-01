#include "table_view.h"
#include "abstract_table_model.h"
#include "iview_model.h"
#include "iresize_policy.h"
#include "item_view_delegate.h"
#include "helpers.h"
#include "command_menu.h"
#include "finally.h"
#include "table_proxy_model.h"
#include "header_view.h"

namespace SeoSpider
{

TableView::TableView(QWidget* parent, bool supportColumSpans, bool sortingEnabled, bool showCustomizeColumnsButton)
	: QTableView(parent)
	, m_model(nullptr)
	, m_viewModel(nullptr)
	, m_contextMenu(nullptr)
	, m_showAdditionalGrid(false)
	, m_rowHeight(Common::Helpers::pointsToPixels(22))
	, m_supportColumnSpans(supportColumSpans)
	, m_sortFilterProxyModel(new TableProxyModel)
	, m_headerView(new HeaderView(this, showCustomizeColumnsButton))
{
	Q_UNUSED(sortingEnabled);

	m_headerView->setSortIndicatorShown(true);
	setHorizontalHeader(m_headerView);

	setMouseTracking(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setItemDelegate(new ItemViewDelegate(nullptr, this));
	horizontalHeader()->setSectionsMovable(true);
	setShowGrid(false);
#ifdef USE_SORTING
	if (sortingEnabled)
	{
		setSortingEnabled(true);
		QTableView::sortByColumn(0, Qt::AscendingOrder);
	}
#endif

	qRegisterMetaType<QModelIndexList>();
}

void TableView::setModel(QAbstractItemModel* model)
{
	m_model = Common::Helpers::fast_cast<AbstractTableModel*>(model);

#ifdef USE_SORTING
	m_sortFilterProxyModel->setSourceModel(model);
	QTableView::setModel(m_sortFilterProxyModel);
	if (m_viewModel)
	{
		// reconnect signals/slots with correct order
		setViewModel(m_viewModel);
	}
#else
	QTableView::setModel(model);
#endif

	applyRowHeight();

	if (m_model->resizePolicy())
	{
		IResizePolicy* prevPolicy = m_model->resizePolicy();
		m_model->resizePolicy()->init(this, prevPolicy);

		VERIFY(connect(m_model->resizePolicy()->qobject(), SIGNAL(columnSizeChanged()), this, SLOT(adjustColumnSize())));
	}

	initSpans();
	m_headerView->initColumns();

	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(initSpans())));
	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(adjustColumnSize())));
	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(applyRowHeight())));

#ifdef USE_SORTING
	VERIFY(connect(m_sortFilterProxyModel, &QSortFilterProxyModel::layoutAboutToBeChanged, this, &TableView::onLayoutAboutToBeChanged));
	VERIFY(connect(m_sortFilterProxyModel, &QSortFilterProxyModel::layoutChanged, this, &TableView::onLayoutChanged));
#endif
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
#ifdef USE_SORTING
		if (index.model() != m_sortFilterProxyModel)
		{
			ASSERT(index.model() == m_model);
			index = m_sortFilterProxyModel->mapFromSource(index);
		}
#endif
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

	m_contextMenu->execFor(event->globalPos(), indexAt(event->pos()));
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

	const auto makeUniqueSelectedRows = [this]() -> std::vector<int>
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

	painter.setPen(viewModel()->gridLineColor(QModelIndex()));

	const QRect viewportRect = viewport()->rect();
	const int maxViewportRowCount = viewportRowCapacity();

	std::vector<int> uniqueSelectedRows = makeUniqueSelectedRows();

	//
	// draw horizontal row grid lines
	//

	const int baseLineYOffset = std::min(rowViewportPosition(rowAt(0)), 0) + m_rowHeight;

	const int columnCount = model()->columnCount();
	const int lastXOffset = columnWidth(columnCount - 1) + columnViewportPosition(columnCount - 1);

	for (int i = 0; i < maxViewportRowCount; ++i)
	{
		const int offsetByY = m_rowHeight * i + baseLineYOffset;

		const int thisRowLogicalIndex = rowAt(offsetByY);
		const int belowRowLogicalIndex = rowAt(offsetByY - m_rowHeight);

		const bool isThisOrNextRowSelected = std::find_if(
			std::begin(uniqueSelectedRows),
			std::end(uniqueSelectedRows),
			[&](int selectedRow) {return selectedRow == thisRowLogicalIndex || selectedRow == belowRowLogicalIndex; }) !=
			std::end(uniqueSelectedRows);

		if (isThisOrNextRowSelected)
		{
			painter.save();

			painter.setPen(viewModel()->selectedGridLineColor(QModelIndex()));
			painter.drawLine(QPoint(0, offsetByY), QPoint(lastXOffset, offsetByY));
			painter.restore();

			painter.drawLine(QPoint(lastXOffset, offsetByY), QPoint(width(), offsetByY));
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
		const int topRowLineYCoordinate = rowViewportPosition(uniqueSelectedRows[i]);

		verticalLineSegments.emplace_back(VerticalLineSegment{ topRowLineYCoordinate, topRowLineYCoordinate + m_rowHeight });
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

void TableView::rowsInserted(const QModelIndex &parent, int first, int last)
{
	applyRowHeightToRowRange(first, last);

	QTableView::rowsInserted(parent, first, last);
}

void TableView::setContextMenu(CommandMenu* menu) noexcept
{
	m_contextMenu.reset(menu);
}

void TableView::setViewModel(IViewModel* modelView) noexcept
{
	if (m_viewModel)
	{
		disconnect(m_viewModel->qobject(), SIGNAL(repaintItems(const QModelIndexList&)),
			this, SLOT(onAboutRepaintItems(const QModelIndexList&)));
	}

	m_viewModel = modelView;
	m_viewModel->qobject()->setParent(this);

	VERIFY(connect(m_viewModel->qobject(), SIGNAL(repaintItems(const QModelIndexList&)),
		this, SLOT(onAboutRepaintItems(const QModelIndexList&))));

	Common::Helpers::fast_cast<ItemViewDelegate*>(itemDelegate())->setViewModel(m_viewModel);
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

void TableView::recalculateColumnsSize()
{
	if (m_model && m_model->resizePolicy())
	{
		m_model->resizePolicy()->resize(this);
	}
}

void TableView::initSpans()
{
	if (!m_supportColumnSpans)
	{
		return;
	}

	clearSpans();

	const int rows = model()->rowCount();
	const int columns = model()->columnCount();

	for (int row = 0; row < rows; ++row)
	{
		for (int column = 0; column < columns;)
		{
			QSize colSpan = model()->span(model()->index(row, column));
			Common::Finally finallyObject([&] { column += colSpan.width(); });

			if (colSpan.width() == -1 && colSpan.height() == -1)
			{
				break;
			}

			// skip Qt warnings using this condition
			if (colSpan.width() == 1 && colSpan.height() == 1)
			{
				continue;
			}

			setSpan(row, column, colSpan.height(), colSpan.width());
		}
	}
}

int TableView::viewportRowCapacity() const noexcept
{
	const QRect viewportRect = viewport()->rect();
	const int viewportRowCountCapacity = viewportRect.height() / m_rowHeight + 1;

	return viewportRowCountCapacity;
}

void TableView::adjustColumnSize()
{
	m_model = Common::Helpers::fast_cast<AbstractTableModel*>(m_model);

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

#ifdef USE_SORTING
		if (index.model() == m_model)
		{
			update(m_sortFilterProxyModel->mapFromSource(index));
			continue;
		}


		if (index.row() >= m_sortFilterProxyModel->rowCount() ||
			index.column() >= m_sortFilterProxyModel->columnCount())
		{
			continue;
		}

		// workaround to fix crash caused by missing mapping for this index
		// by getting index from model we can be sure that there is a mapping for this index
		update(m_sortFilterProxyModel->index(index.row(), index.column()));
#else
		update(index);
#endif
	}
}

void TableView::applyRowHeight()
{
	applyRowHeightToRowRange(0, m_model->rowCount());
}

void TableView::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& indices, QAbstractItemModel::LayoutChangeHint hint)
{
	if (indices.isEmpty() && hint == QAbstractItemModel::VerticalSortHint)
	{
		if (model()->rowCount() > 0)
		{
			return;
		}

		const int columns = model()->columnCount();
		// fixing reseting columns width bug after sorting empty table
		for (int i = 0; i < columns; ++i)
		{
			m_columnsizesBeforeSorting << columnWidth(i);
		}
	}
}

void TableView::onLayoutChanged(const QList<QPersistentModelIndex>& indices, QAbstractItemModel::LayoutChangeHint hint)
{
	if (indices.isEmpty() && hint == QAbstractItemModel::VerticalSortHint)
	{
		viewModel()->invalidateItemViewRendererCache();


#ifdef USE_SORTING
		// seems like a bug in QSortProxyFiltermodel, so we need a costyl here
		// after resorting each row has undefined height, but it should have a specific height: see applyRowHeightToRowRange
		// TODO: fix it in a proper way
		// check it in the future versions of Qt, probably it will be fixed
		applyRowHeight();
#endif;

		// fixing reseting columns width bug after sorting empty table
		int index = 0;
		foreach(int size, m_columnsizesBeforeSorting)
		{
			setColumnWidth(index, size);
			++index;
		}

		m_columnsizesBeforeSorting.clear();
	}
}

void TableView::applyRowHeightToRowRange(int first, int last)
{
	for (int i = first; i <= last; ++i)
	{
		setRowHeight(i, m_rowHeight);
	}
}

}
