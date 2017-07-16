#pragma once

#include "ui_main_frame.h"

namespace QuickieWebBot
{

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);

	void showListView();
	void showSummaryView();

private:
	void initialize();

private:
	Ui::MainFrameClass m_ui;
};

}