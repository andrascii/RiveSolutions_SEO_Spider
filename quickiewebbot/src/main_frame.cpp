#include "application.h"
#include "main_frame.h"
#include "model_controller.h"
#include "data_collection.h"
#include "web_crawler.h"
#include "quickie_web_bot_helpers.h"
#include "data_pages_widget.h"
#include "control_panel_widget.h"
#include "action_keys.h"
#include "action_registry.h"
#include "main_frame_menu_bar.h"
#include "service_locator.h"
#include "settings_page_registry.h"
#include "titled_widget.h"


namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_applicationSettingsWidget(nullptr)
{
	init();
}

void MainFrame::showApplicationSettingsWidget()
{
	if (!m_applicationSettingsWidget)
	{
		m_applicationSettingsWidget = new ApplicationSettingsWidget(this);
	}

	m_applicationSettingsWidget->exec();
}

void MainFrame::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);

	Q_EMIT onAboutMainFrameToClose();
}

void MainFrame::init()
{
	createActions();
	createAndSetCentralWidget();

	setWindowIcon(QIcon(QStringLiteral(":/images/robot.ico")));

	setMenuBar(new MainFrameMenuBar(this));
	setStatusBar(new QStatusBar(this));
}

void MainFrame::createActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// file actions
	actionRegistry.addActionGroup(s_fileActionGroup);

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_openFileAction, 
		QIcon(QStringLiteral(":/images/open-file-icon.png")), "Open File");

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_closeFileAction, "Close File");

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_recentFilesAction, 
		QIcon(QStringLiteral(":/images/actions-document-open-recent-icon.png")), "Recent Files");

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAction, 
		QIcon(QStringLiteral(":/images/save-icon.png")), "Save File");

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAsAction, 
		QIcon(QStringLiteral(":/images/save-as-icon.png")), "Save File As");

	actionRegistry.addGlobalAction(s_exitProgramAction, "Close");

	// settings actions
	actionRegistry.addActionGroup(s_settingsActionGroup);

	m_openSettingsWidget = actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openSettingsAction,
		QIcon(QStringLiteral(":/images/setting.png")), "Settings");

	VERIFY(connect(m_openSettingsWidget, SIGNAL(triggered()), this, SLOT(showApplicationSettingsWidget())));
}

void MainFrame::createAndSetCentralWidget()
{
	QWidget* centralWidget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	layout->setSpacing(0);

	layout->addWidget(new DataPagesWidget(centralWidget));

	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);
}

}