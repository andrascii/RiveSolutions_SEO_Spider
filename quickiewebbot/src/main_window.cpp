#include "application.h"
#include "application_settings_widget.h"
#include "main_window.h"
#include "quickie_web_bot_helpers.h"
#include "data_pages_widget.h"
#include "action_keys.h"
#include "action_registry.h"
#include "menu_bar.h"
#include "settings_page_impl.h"
#include "crawler_options.h"
#include "crawler.h"
#include "host_info.h"
#include "ui_crawler_settings_widget.h"
#include "ui_proxy_settings_widget.h"
#include "ui_limits_settings_widget.h"
#include "ui_preferences_settings_widget.h"
#include "ui_language_settings_widget.h"

namespace QuickieWebBot
{

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_applicationSettingsWidget(nullptr)
	, m_initialized(false)
{
}

void MainWindow::showApplicationSettingsWidget()
{
	if (!m_applicationSettingsWidget)
	{
		m_applicationSettingsWidget = new ApplicationSettingsWidget(this);
	}

	m_applicationSettingsWidget->exec();
}

void MainWindow::showMessageBoxDialog(const QString& title, const QString& message, MessageBoxDialog::Icon icon)
{
	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog(this);
	messageBoxDialog->setAttribute(Qt::WA_DeleteOnClose, true);

	messageBoxDialog->setWindowTitle(title);
	messageBoxDialog->setMessage(message);
	messageBoxDialog->setIcon(icon);

	messageBoxDialog->show();
}

void MainWindow::init()
{
	DEBUG_ASSERT(!m_initialized);

	createActions();
	createAndSetCentralWidget();
	registerSettingsPages();

	setWindowIcon(QIcon(QStringLiteral(":/images/robot.ico")));

	setMenuBar(new MenuBar(this));
	setStatusBar(new QStatusBar(this));

	m_initialized = true;
}

void MainWindow::createActions()
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
		QIcon(QStringLiteral(":/images/settings.png")), tr("Settings"));

	// crawler actions
	actionRegistry.addGlobalAction(s_startCrawlerAction, tr("Start Crawler"));
	actionRegistry.addGlobalAction(s_stopCrawlerAction, tr("Stop Crawler"));
	actionRegistry.addGlobalAction(s_clearCrawledDataAction, tr("Clear Crawled Data"));


	VERIFY(connect(actionRegistry.globalAction(s_openSettingsAction), SIGNAL(triggered()), this, SLOT(showApplicationSettingsWidget())));

	VERIFY(connect(actionRegistry.globalAction(s_startCrawlerAction), SIGNAL(triggered()), this, SLOT(startCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_stopCrawlerAction), SIGNAL(triggered()), this, SLOT(stopCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_clearCrawledDataAction), SIGNAL(triggered()), this, SLOT(clearCrawledData())));
}

void MainWindow::createAndSetCentralWidget()
{
	QWidget* centralWidget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	layout->setSpacing(0);
	layout->setMargin(0);

	layout->addWidget(new DataPagesWidget(centralWidget));

	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);
}

void MainWindow::registerSettingsPages() const
{
	SettingsPageImpl<Ui_CrawlerSettingsWidget>::registerSettingsPage(QIcon(":/images/crawler-settings.png"), TYPE_STRING(Ui_CrawlerSettingsWidget));
	SettingsPageImpl<Ui_ProxySettingsWidget>::registerSettingsPage(QIcon(":/images/proxy-settings.png"), TYPE_STRING(Ui_ProxySettingsWidget));
	SettingsPageImpl<Ui_LimitsSettingsWidget>::registerSettingsPage(QIcon(":/images/limits-settings.png"), TYPE_STRING(Ui_LimitsSettingsWidget));
	SettingsPageImpl<Ui_PreferencesSettingsWidget>::registerSettingsPage(QIcon(":/images/preferences-settings-icon.png"), TYPE_STRING(Ui_PreferencesSettingsWidget));
	SettingsPageImpl<Ui_LanguageSettingsWidget>::registerSettingsPage(QIcon(":/images/lang-settings.png"), TYPE_STRING(Ui_LanguageSettingsWidget));
}

void MainWindow::startCrawler()
{
	HostInfo hostInfo(theApp->preferences()->url().host().toLatin1());

	if (!hostInfo.isValid())
	{
		showMessageBoxDialog("DNS Lookup Failed!",
			"I'm sorry but I cannot to find this website.\n"
			"Please, be sure that you entered a valid address.", 
			MessageBoxDialog::WarningIcon);

		return;
	}

	WebCrawler::CrawlerOptions options;

	// preferences
	options.host = theApp->preferences()->url();
	options.minTitleLength = theApp->preferences()->minTitleLength();
	options.maxTitleLength = theApp->preferences()->maxTitleLength();
	options.limitMaxUrlLength = theApp->preferences()->limitMaxUrlLength();
	options.maxDescriptionLength = theApp->preferences()->maxDescriptionLength();
	options.minDescriptionLength = theApp->preferences()->minDescriptionLength();
	options.maxH1LengthChars = theApp->preferences()->maxH1LengthChars();
	options.maxH2LengthChars = theApp->preferences()->maxH2LengthChars();
	options.maxImageAltTextChars = theApp->preferences()->maxImageAltTextChars();
	options.maxImageSizeKb = theApp->preferences()->maxImageSize();

	// crawler settings
	options.checkExternalLinks = theApp->preferences()->checkExternalUrls();
	options.followInternalNofollow = theApp->preferences()->followInternalNoFollow();
	options.followExternalNofollow = theApp->preferences()->followExternalNoFollow();
	options.checkSubdomains = theApp->preferences()->checkSubdomains();
	options.followRobotsTxtRules = theApp->preferences()->followRobotsTxt();
	options.checkImages = theApp->preferences()->checkImages();
	options.checkCss = theApp->preferences()->checkCSS();
	options.checkJavaScript = theApp->preferences()->checkJavaScript();
	options.checkSwf = theApp->preferences()->checkSWF();

	options.parserTypeFlags.setFlag(WebCrawler::JavaScriptResourcesParserType);
	options.parserTypeFlags.setFlag(WebCrawler::CssResourcesParserType);
	options.parserTypeFlags.setFlag(WebCrawler::ImagesResourcesParserType);
	options.parserTypeFlags.setFlag(WebCrawler::VideoResourcesParserType);
	options.parserTypeFlags.setFlag(WebCrawler::FlashResourcesParserType);

	theApp->crawler()->startCrawling(options);

	emit crawlerStarted();
}

void MainWindow::stopCrawler()
{
	theApp->crawler()->stopCrawling();

	emit crawlerStopped();
}

void MainWindow::clearCrawledData()
{
}

}