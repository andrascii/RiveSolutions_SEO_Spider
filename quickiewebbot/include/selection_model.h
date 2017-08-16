#pragma once

namespace QuickieWebBot
{

class TableView;

class SelectionModel : public QItemSelectionModel
{
	Q_OBJECT

public:
	SelectionModel(TableView* parent);

protected:
	Q_SLOT virtual void select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command) override;
	Q_SLOT virtual void select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command) override;
	Q_SLOT virtual void setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command) override;
	
private:
	TableView* m_gridView;
};

}