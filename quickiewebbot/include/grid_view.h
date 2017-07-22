#pragma once

namespace QuickieWebBot
{

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

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;

	Q_SLOT void onModelDataAccessorChanged(IModelDataAccessor* accessor);

private:
	void updateColumnsSpan();
	void updateCursor(int flags);

private:
	IModelDataAccessor* m_accessor;
	QModelIndex m_hoveredIndex;
	std::unique_ptr<IGridViewResizeStrategy> m_resizeStrategy;
	bool m_isCursorOverriden;
};


}