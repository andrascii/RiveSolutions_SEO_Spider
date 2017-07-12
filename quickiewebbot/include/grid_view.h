#pragma once

namespace QuickieWebBot
{

class IModelDataAccessor;

class GridView : public QTableView
{
	Q_OBJECT

public:
	explicit GridView(QWidget* parent = nullptr);

	virtual void setModel(QAbstractItemModel* model) override;

	IModelDataAccessor* dataAccessor();

protected:
	virtual void paintEvent(QPaintEvent* event) override;

	Q_SLOT void onModelAccessorChanged(IModelDataAccessor* accessor);

private:
	IModelDataAccessor* m_accessor;
};


}