#pragma once

namespace SeoSpider
{

class ISummaryDataAccessor;

class FilterTableSelectionModel : public QItemSelectionModel
{
	Q_OBJECT

public:
	FilterTableSelectionModel(QAbstractItemModel* model = nullptr);
	FilterTableSelectionModel(QAbstractItemModel* model, QObject* parent);

private slots:
	void onAboutSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void onSummaryDataAccessorSelectionChanged(int row);

protected:
	Q_SLOT virtual void select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command) override;
	Q_SLOT virtual void select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command) override;
	Q_SLOT virtual void setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command) override;

private:
	void init();

private:
	ISummaryDataAccessor* m_dataAccessor;
};

}