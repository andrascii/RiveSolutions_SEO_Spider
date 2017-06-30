#pragma once

#include "main_frame.h"

namespace QuickieWebBot
{

class ModelController;

class Application : public QApplication
{
	Q_OBJECT

public:
	static Application* instance();
	Application(int& argc, char** argv);

private:
	void initialize();
	void initializeStyleSheet();

private:
	static Application* s_app;

	ModelController* m_modelController;
	std::unique_ptr<MainFrame> m_mainFrame;
};

}
