#include "main_frame_controller.h"
#include "main_frame.h"

namespace QuickieWebBot
{

MainFrameController::MainFrameController(MainFrame* mainFrame, QObject* parent)
	: QObject(parent)
	, m_mainFrame(mainFrame)
{
}

bool MainFrameController::eventFilter(QObject* object, QEvent* event)
{
	return false;
}

}