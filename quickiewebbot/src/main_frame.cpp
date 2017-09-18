#include "application.h"
#include "application_settings_widget.h"
#include "preferences.h"
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
		QIcon(QStringLiteral(":/images/open-file-icon.png")), tr("Open File"));

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_closeFileAction, tr("Close File"));

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_recentFilesAction, 
		QIcon(QStringLiteral(":/images/actions-document-open-recent-icon.png")), tr("Recent Files"));

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAction, 
		QIcon(QStringLiteral(":/images/save-icon.png")), tr("Save File"));

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAsAction, 
		QIcon(QStringLiteral(":/images/save-as-icon.png")), tr("Save File As"));

	actionRegistry.addGlobalAction(s_exitProgramAction, tr("Close"));

	// settings actions

	actionRegistry.addGlobalAction(s_openSettingsAction,
		QIcon(QStringLiteral(":/images/setting.png")), tr("Settings"));

	// crawler actions
	actionRegistry.addGlobalAction(s_startCrawlerAction, tr("Start Crawler"));
	actionRegistry.addGlobalAction(s_stopCrawlerAction, tr("Stop Crawler"));
	actionRegistry.addGlobalAction(s_clearCrawledDataAction, tr("Clear Crawled Data"));


	VERIFY(connect(actionRegistry.globalAction(s_openSettingsAction), SIGNAL(triggered()), this, SLOT(showApplicationSettingsWidget())));

	VERIFY(connect(actionRegistry.globalAction(s_startCrawlerAction), SIGNAL(triggered()), this, SLOT(startCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_stopCrawlerAction), SIGNAL(triggered()), this, SLOT(stopCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_clearCrawledDataAction), SIGNAL(triggered()), this, SLOT(clearCrawledData())));
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

void MainFrame::startCrawler()
{
	WebCrawler::WebCrawlerOptions options;

	options.url = theApp->preferences()->url();
	options.minTitleLength = theApp->preferences()->minTitleLength();
	options.maxTitleLength = theApp->preferences()->maxTitleLength();
	options.limitMaxUrlLength = theApp->preferences()->limitMaxUrlLength();
	options.maxDescriptionLength = theApp->preferences()->maxDescriptionLength();
	options.minDescriptionLength = theApp->preferences()->minDescriptionLength();
	options.maxH1LengthChars = theApp->preferences()->maxH1LengthChars();
	options.maxH2LengthChars = theApp->preferences()->maxH2LengthChars();
	options.maxImageAltTextChars = theApp->preferences()->maxImageAltTextChars();
	options.maxImageSizeKb = theApp->preferences()->maxImageSize();

	theApp->webCrawler()->startCrawling(options);
}

void MainFrame::stopCrawler()
{
	theApp->webCrawler()->stopCrawling();
}

void MainFrame::clearCrawledData()
{

}

}