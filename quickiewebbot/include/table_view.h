#pragma once

namespace QuickieWebBot
{

class ITableModel;
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

	void setContextMenu(ContextMenuDataCollectionRow* menu) noexcept;
	void setViewModel(IViewModel* modelView) noexcept;
	const IViewModel* viewModel() const noexcept;
	QModelIndex hoveredIndex() const noexcept;

	Q_SIGNAL void selectionWasChanged(const QItemSelection& selected, const QItemSelection& deselected);

protected:
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;

	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;

private:
	void initSpan();
	void selectRow(const QPoint& point);

	Q_SLOT void adjustColumnSize();

private:
	ITableModel* m_model;

	IViewModel* m_viewModel;

	QModelIndex m_hoveredIndex;

	QMenu* m_contextMenu;

	bool m_isCursorOverriden;
};

}