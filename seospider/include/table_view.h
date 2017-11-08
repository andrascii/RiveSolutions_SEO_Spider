#pragma once

namespace SeoSpider
{

class AbstractTableModel;
class IGridDataAccessor;
class IResizePolicy;
class ContextMenuDataCollectionRow;
class IRenderer;
class IViewModel;

class TableView : public QTableView
{
	Q_OBJECT

public:
	explicit TableView(QWidget* parent = nullptr);

	virtual void setModel(QAbstractItemModel* model) override;

	IViewModel* viewModel() const noexcept;
	void setViewModel(IViewModel* modelView) noexcept;
	
	void setContextMenu(QMenu* menu) noexcept;

	void setShowAdditionalGrid(bool value) noexcept;
	bool showAdditionalGrid() const noexcept;

	Q_SLOT void initSpans();

protected:
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) override;

	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
	virtual void rowsInserted(const QModelIndex &parent, int first, int last) override;

private:
	Q_SLOT void adjustColumnSize();
	Q_SLOT void onAboutRepaintItems(const QModelIndexList& modelIndexes);

	Q_SLOT void applyRowHeight();

	void applyRowHeightToRowRange(int first, int last);

private:
	AbstractTableModel* m_model;
	IViewModel* m_viewModel;
	QMenu* m_contextMenu;

	bool m_showAdditionalGrid;
	int m_rowHeight;
};

}