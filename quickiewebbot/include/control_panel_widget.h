#pragma once

#include "ui_control_panel_widget.h"

namespace QuickieWebBot
{

class ControlPanelWidget : public QWidget
{
	Q_OBJECT

public:
	ControlPanelWidget(QWidget* parent);

private:
	Q_SLOT void setUrl();
	Q_SLOT void startCrawling();

private:
	void initialize();

private:
	Ui::ControlPanelWidget m_ui;
};

}