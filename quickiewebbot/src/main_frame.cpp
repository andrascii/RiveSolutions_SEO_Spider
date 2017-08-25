#include "application.h"
#include "main_frame.h"
#include "model_controller.h"
#include "data_collection.h"
#include "web_crawler.h"
#include "quickie_web_bot_helpers.h"
#include "main_frame_controller.h"
#include "data_pages_widget.h"
#include "control_panel_widget.h"
#include "action_keys.h"
#include "action_registry.h"
#include "main_frame_menu_bar.h"
#include "service_locator.h"
#include "settings_page_registry.h"

#include "limits_settings_dialog.h"
#include "proxy_settings_dialog.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_mainFrameController(new MainFrameController(this, this))
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

	m_applicationSettingsWidget->show();
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
	createSettingsPages();

	setWindowIcon(QIcon(QStringLiteral(":/images/robot.ico")));

	setMenuBar(new MainFrameMenuBar(this));
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

void MainFrame::createSettingsPages()
{
	ServiceLocator::instance()->service<SettingsPageRegistry>()->registerSettingsPage(TYPE_STRING(CrawlerSettingsWidget), 
		new CrawlerSettingsWidget());

	ServiceLocator::instance()->service<SettingsPageRegistry>()->registerSettingsPage(TYPE_STRING(LimitsSettingsDialog),
		new LimitsSettingsDialog());

	ServiceLocator::instance()->service<SettingsPageRegistry>()->registerSettingsPage(TYPE_STRING(ProxySettingsDialog),
		new ProxySettingsDialog());
}

void MainFrame::createAndSetCentralWidget()
{
	QWidget* centralWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(centralWidget);

	layout->addWidget(new ControlPanelWidget(centralWidget));
	layout->addWidget(new DataPagesWidget(centralWidget));

	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);
}

}