#pragma once

#include "grid_data_accessor_factory.h"

namespace QuickieWebBot
{

class GridModel;
class IGridDataAccessor;
class IGridViewResizeStrategy;
class ContextMenuDataCollectionRow;

class GridView : public QTableView
{
	Q_OBJECT

public:
	explicit GridView(QWidget* parent = nullptr);

	virtual void setModel(QAbstractItemModel* model) override;

	IGridDataAccessor* modelDataAccessor();
	QModelIndex hoveredIndex() const;

	void setContextMenu(ContextMenuDataCollectionRow* menu);

	Q_SLOT void setParams(const GridDataAccessorFactoryParams& params);
	Q_SIGNAL void childViewParamsChanged(const GridDataAccessorFactoryParams& params);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

	Q_SLOT void onModelDataAccessorChanged(IGridDataAccessor* accessor, IGridDataAccessor* oldAccessor);

private:
	void updateColumnsSpan();
	void updateCursor(int flags);
	void selectRow(const QPoint& point);

private:
	GridModel* m_gridViewModel;
	QModelIndex m_hoveredIndex;
	bool m_isCursorOverriden;
	ContextMenuDataCollectionRow* m_contextMenu;
};

}