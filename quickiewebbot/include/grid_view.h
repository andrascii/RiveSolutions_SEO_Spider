#pragma once

namespace QuickieWebBot
{

class IGridModel;
class IGridDataAccessor;
class IGridViewResizeStrategy;
class ContextMenuDataCollectionRow;
class IGridViewPainter;

class GridView : public QTableView
{
	Q_OBJECT

public:
	explicit GridView(QWidget* parent = nullptr);

	virtual void setModel(QAbstractItemModel* model) override;

	void setContextMenu(ContextMenuDataCollectionRow* menu);

	//
	// TODO: make it pure virtual
	//
	QList<IGridViewPainter*> painters() const noexcept;

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
	IGridModel* m_gridViewModel;
	QModelIndex m_hoveredIndex;
	bool m_isCursorOverriden;
	ContextMenuDataCollectionRow* m_contextMenu;
};

}