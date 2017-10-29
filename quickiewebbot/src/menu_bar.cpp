#include "menu_bar.h"
#include "action_registry.h"
#include "action_keys.h"

namespace QuickieWebBot
{

MenuBar::MenuBar(QWidget* parent)
	: QMenuBar(parent)
{
	init();
}

void MenuBar::init()
{
	addMenu(buildMenuFile());
	addMenu(buildMenuSettings());
}

QMenu* MenuBar::buildMenuFile()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QMenu* fileMenu = new QMenu(tr("File"), this);

	fileMenu->addAction(actionRegistry.globalAction(s_openFileAction));
	fileMenu->addAction(actionRegistry.globalAction(s_recentFilesAction));
	fileMenu->addSeparator();
	fileMenu->addAction(actionRegistry.globalAction(s_saveFileAction));
	fileMenu->addAction(actionRegistry.globalAction(s_saveFileAsAction));
	fileMenu->addAction(actionRegistry.globalAction(s_closeFileAction));
	fileMenu->addSeparator();
	fileMenu->addAction(actionRegistry.globalAction(s_exitProgramAction));

	return fileMenu;
}

QMenu* MenuBar::buildMenuSettings()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QMenu* settingsMenu = new QMenu(tr("Settings"), this);

	settingsMenu->addAction(actionRegistry.globalAction(s_openSettingsAction));

	return settingsMenu;
}

}