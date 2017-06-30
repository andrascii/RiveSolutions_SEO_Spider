#pragma once

#include "main_frame.h"

namespace QuickieWebBot
{

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int argc, char** argv);

private:
	void initializeStyleSheet();

private:
	std::unique_ptr<MainFrame> m_mainFrame;
};

}
