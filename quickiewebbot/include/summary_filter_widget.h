#pragma once

#include "ui_summary_filter_widget.h"
#include "summary_data_accessor_factory.h"

namespace QuickieWebBot
{

class TableView;

class SummaryFilterWidget : public QFrame
{
	Q_OBJECT

public:
	SummaryFilterWidget(QWidget* parent = nullptr);

	void setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType);

private:
	Q_SLOT void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void init();
	void initSummaryView();
	void initDetailsView();

private:
	std::unique_ptr<Ui::SummaryFilterWidget> m_ui;
};

}
