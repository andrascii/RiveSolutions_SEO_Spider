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
	void setColumnResizeStrategy(std::unique_ptr<IGridViewResizeStrategy> strategy);

	IModelDataAccessor* dataAccessor();

	QModelIndex hoveredIndex() const;

	Q_SLOT void setParams(const ModelDataAccessorFactoryParams& params);
	Q_SIGNAL void childViewParamsChanged(const ModelDataAccessorFactoryParams& params);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;

	Q_SLOT void onModelDataAccessorChanged(IModelDataAccessor* accessor);

private:
	void updateColumnsSpan();
	void updateCursor(int flags);

private:
	GridViewModel* m_gridViewModel;
	IModelDataAccessor* m_accessor;
	QModelIndex m_hoveredIndex;
	std::unique_ptr<IGridViewResizeStrategy> m_resizeStrategy;
	bool m_isCursorOverriden;
};


}