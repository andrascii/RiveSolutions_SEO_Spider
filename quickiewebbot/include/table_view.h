#pragma once

namespace QuickieWebBot
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

	void setContextMenu(QMenu* menu) noexcept;
	void setViewModel(IViewModel* modelView) noexcept;
	IViewModel* viewModel() const noexcept;

	void initSpan();

protected:
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;

private:
	void selectRow(const QPoint& point);

	Q_SLOT void adjustColumnSize();
	Q_SLOT void onAboutRepaintItems(const QModelIndexList& modelIndexes);

private:
	AbstractTableModel* m_model;

	IViewModel* m_viewModel;

	QMenu* m_contextMenu;

	bool m_isCursorOverriden;
};

}