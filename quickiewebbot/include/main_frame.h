#pragma once

#include "ui_main_frame.h"

namespace QuickieWebBot
{

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);


private:
	void initialize();

private:
	Ui::MainFrameClass ui;
};

}