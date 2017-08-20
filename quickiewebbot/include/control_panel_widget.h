#pragma once

#include "ui_control_panel.h"

namespace QuickieWebBot
{

class ControlPanelWidget : public QFrame
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
	Ui::ControlPanel m_ui;
};

}