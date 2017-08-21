#pragma once

#include "ui_errors_filter_widget.h"

namespace QuickieWebBot
{

class TableView;

class ErrorsFilterWidget : public QFrame
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
	std::unique_ptr<Ui::ErrorsFilterWidget> m_ui;
};

}
