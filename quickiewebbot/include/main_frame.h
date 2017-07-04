#pragma once

#include "ui_main_frame.h"

namespace QuickieWebBot
{

class ModelControllerData;

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(ModelControllerData* data, QWidget *parent = 0);


private:
	void init();

private:
	Ui::MainFrameClass ui;
	ModelControllerData* m_data;
};

}