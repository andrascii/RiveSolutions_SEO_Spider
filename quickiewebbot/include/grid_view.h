#pragma once

namespace QuickieWebBot
{

class IGridModel;
class IGridDataAccessor;
class IGridViewResizePolicy;
class ContextMenuDataCollectionRow;
class IRenderer;
class IGridViewModel;

class GridView : public QTableView
{
	Q_OBJECT

public:
	explicit GridView(QWidget* parent = nullptr);

	virtual void setModel(QAbstractItemModel* model) override;

	void setContextMenu(ContextMenuDataCollectionRow* menu);

	void setViewModel(IGridViewModel* modelView);
	const IGridViewModel* viewModel() const noexcept;

	QModelIndex hoveredIndex() const;

protected:
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
	void initSpan();
	void selectRow(const QPoint& point);

private:
	IGridModel* m_model;

	IGridViewModel* m_viewModel;

	QModelIndex m_hoveredIndex;

	QMenu* m_contextMenu;

	bool m_isCursorOverriden;
};

}