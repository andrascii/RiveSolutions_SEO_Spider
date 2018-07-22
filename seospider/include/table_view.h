
#pragma once

namespace SeoSpider
{

class AbstractTableModel;
class IGridDataAccessor;
class IResizePolicy;
class ContextMenuDataCollectionRow;
class IRenderer;
class IViewModel;
class CommandMenu;

class IView
{
public:
	virtual int viewportRowCapacity() const noexcept = 0;
};

class TableView : public QTableView, public IView
{
	Q_OBJECT

public:
	explicit TableView(QWidget* parent, bool supportColumSpans, bool sortingEnabled);

	virtual void setModel(QAbstractItemModel* model) override;
	IViewModel* viewModel() const noexcept;
	void setViewModel(IViewModel* modelView) noexcept;
	void setContextMenu(CommandMenu* menu) noexcept;
	void setShowAdditionalGrid(bool value) noexcept;
	bool showAdditionalGrid() const noexcept;
	void recalculateColumnsSize();

	// IView impl
	virtual int viewportRowCapacity() const noexcept override;

public slots:
	void adjustColumnSize();
	void initSpans();

protected:
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
	virtual void rowsInserted(const QModelIndex &parent, int first, int last) override;

private slots:
	void onAboutRepaintItems(const QModelIndexList& modelIndexes);
	void applyRowHeight();

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>&, QAbstractItemModel::LayoutChangeHint);
	void onLayoutChanged(const QList<QPersistentModelIndex>&, QAbstractItemModel::LayoutChangeHint);

private:
	void applyRowHeightToRowRange(int first, int last);

private:
	AbstractTableModel* m_model;
	QSortFilterProxyModel* m_sortFilterProxyModel;
	IViewModel* m_viewModel;
	std::unique_ptr<CommandMenu> m_contextMenu;

	bool m_showAdditionalGrid;
	int m_rowHeight;
	bool m_supportColumnSpans;
	QList<int> m_columnsizesBeforeSorting;
};

}