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

#ifdef QT_DEBUG
#include "debug_info_web_page_widget.h"
#endif // DEBUG



namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_mainFrameController(new MainFrameController(this, this))
	, m_proxySettingsDialog(nullptr)
	, m_limitsSettingsDialog(nullptr)
{
	init();
}

void MainFrame::showProxySettingsDialog()
{
	if (!m_proxySettingsDialog)
	{
		m_proxySettingsDialog = new ProxySettingsDialog(this);
	}

	m_proxySettingsDialog->exec();
}

void MainFrame::showLimitsSettingsDialog()
{
	if (!m_limitsSettingsDialog)
	{
		m_limitsSettingsDialog = new LimitsSettingsDialog(this);
	}

	m_limitsSettingsDialog->exec();
}

void MainFrame::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);

#ifdef QT_DEBUG

	m_debugWebPageInfoWidget->close();

#endif // DEBUG

}

void MainFrame::init()
{
	createActions();
	createAndSetCentralWidget();

	setWindowIcon(QIcon(QStringLiteral(":/images/robot.ico")));

#ifdef QT_DEBUG

	m_debugWebPageInfoWidget.reset(new DebugInfoWebPageWidget(nullptr));
	m_debugWebPageInfoWidget->show();

#endif // DEBUG

	setMenuBar(new MainFrameMenuBar(this));
}

void MainFrame::createActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// file actions
	actionRegistry.addActionGroup(s_fileActionGroup);
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_openFileAction, "Open File");
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_closeFileAction, "Close File");
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_recentFilesAction, "Recent Files");
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAction, "Save File");
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAsAction, "Save File As");

	actionRegistry.addGlobalAction(s_exitProgramAction, "Close");
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