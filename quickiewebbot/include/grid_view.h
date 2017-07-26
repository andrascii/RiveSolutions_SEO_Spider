#pragma once

#include "model_data_accessor_factory.h"

namespace QuickieWebBot
{

class GridViewModel;
class IModelDataAccessor;
class IGridViewResizeStrategy;

class GridView : public QTableView
{
	Q_OBJECT

public:
	explicit GridView(QWidget* parent = nullptr);

	virtual void setModel(QAbstractItemModel* model) override;

	IModelDataAccessor* dataAccessor();
	QModelIndex hoveredIndex() const;

	void setContextMenu(QMenu* menu);

	Q_SLOT void setParams(const ModelDataAccessorFactoryParams& params);
	Q_SIGNAL void childViewParamsChanged(const ModelDataAccessorFactoryParams& params);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	virtual void mousePressEvent(QMouseEvent* event) override;

	Q_SLOT void onModelDataAccessorChanged(IModelDataAccessor* accessor, IModelDataAccessor* oldAccessor);

private:
	void updateColumnsSpan();
	void updateCursor(int flags);
	void selectRow(const QPoint& point);

private:
	GridViewModel* m_gridViewModel;
	QModelIndex m_hoveredIndex;
	bool m_isCursorOverriden;
	QMenu* m_contextMenu;
};

}