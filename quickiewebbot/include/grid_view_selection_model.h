#pragma once

namespace QuickieWebBot
{

class GridView;

class GridViewSelectionModel : public QItemSelectionModel
{
	Q_OBJECT
public:
	GridViewSelectionModel(GridView* parent);

protected:
	Q_SLOT virtual void select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command) override;
	Q_SLOT virtual void select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command) override;
	Q_SLOT virtual void setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command) override;
	
private:
	GridView* m_gridView;
};

}