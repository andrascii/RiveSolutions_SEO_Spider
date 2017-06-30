#include "main_frame.h"
#include "model_controller.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_modelController(new ModelController(this))
{
	initialize();
}

void MainFrame::initialize()
{
	ui.setupUi(this);

	VERIFY(connect(ui.actionAbout, &QAction::triggered, qApp, &QApplication::aboutQt));

	ui.viewTypeComboBox->addItem(tr("List"));
	ui.viewTypeComboBox->addItem(tr("Tree"));
}

}