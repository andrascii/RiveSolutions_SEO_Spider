#pragma once

namespace QuickieWebBot
{

class TableView;

class ErrorsFilterWidget : public QWidget
{
	Q_OBJECT

public:
	ErrorsFilterWidget(QWidget* parent = nullptr);

private:
	Q_SLOT void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void init();
	void initSummaryView();
	void initDetailsView();

private:
	TableView* m_summaryTableView;
	TableView* m_summaryDetailsTableView;
};

}
