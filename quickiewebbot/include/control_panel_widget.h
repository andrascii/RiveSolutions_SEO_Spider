#pragma once

#include "ui_control_panel_widget.h"

namespace QuickieWebBot
{

class ControlPanelWidget : public QWidget
{
	Q_OBJECT

public:
	ControlPanelWidget(QWidget* parent);

	Q_SLOT void showListView(bool value);
	Q_SLOT void showSummaryView(bool value);

private:
	Q_SLOT void setHost();
	Q_SLOT void startCrawling();

private:
	void initialize();

private:
	Ui::ControlPanelWidget m_ui;
};

}