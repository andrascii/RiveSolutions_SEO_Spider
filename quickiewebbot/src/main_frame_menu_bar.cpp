#include "main_frame_menu_bar.h"
#include "action_registry.h"
#include "action_keys.h"

namespace QuickieWebBot
{

MainFrameMenuBar::MainFrameMenuBar(QWidget* parent)
	: QMenuBar(parent)
{
	init();
}

void MainFrameMenuBar::init()
{
	addMenu(buildMenuFile());
}

QMenu* MainFrameMenuBar::buildMenuFile()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QMenu* fileMenu = new QMenu(tr("File"), this);

	fileMenu->addAction(actionRegistry.globalAction(s_openFileAction));
	fileMenu->addAction(actionRegistry.globalAction(s_recentFilesAction));
	fileMenu->addSeparator();
	fileMenu->addAction(actionRegistry.globalAction(s_saveFileAction));
	fileMenu->addAction(actionRegistry.globalAction(s_saveFileAsAction));
	fileMenu->addAction(actionRegistry.globalAction(s_closeFileAction));

	return fileMenu;
}

}