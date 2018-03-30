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

private:
	void init();

private:
	ISummaryDataAccessor* m_dataAccessor;
};

}