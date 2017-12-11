#include "application.h"
#include "application_settings_widget.h"
#include "main_window.h"
#include "data_pages_widget.h"
#include "action_keys.h"
#include "action_registry.h"
#include "menu_bar.h"
#include "settings_page_impl.h"
#include "crawler.h"
#include "site_map_creator_widget.h"
#include "crawler_status_info.h"
#include "page_factory.h"
#include "settings_page.h"
#include "ui_crawler_settings_widget.h"
#include "ui_proxy_settings_widget.h"
#include "ui_limits_settings_widget.h"
#include "ui_preferences_settings_widget.h"
#include "ui_language_settings_widget.h"
#include "ui_user_agent_settings_widget.h"
#include "user_agent_settings_widget.h"

namespace SeoSpider
{

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_initialized(false)
{
	setWindowTitle(
		theApp->softwareBrandingOptions()->organizationName() + QStringLiteral(" ") +
		theApp->softwareBrandingOptions()->productName()
	);
}

void MainWindow::showSitemapCreatorDialog()
{
	SitemapCreatorWidget* titledWindow = new SitemapCreatorWidget(this);
	titledWindow->exec();
	titledWindow->deleteLater();
}

void MainWindow::showSaveFileDialog()
{
	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		theApp->mainWindow()->showMessageBoxDialog(tr("Error"), tr("Cannot save the document while crawler is working!"),
			MessageBoxDialog::CriticalErrorIcon, QDialogButtonBox::Ok);

		return;
	}

	if (theApp->crawler()->isNoData())
	{
		theApp->mainWindow()->showMessageBoxDialog(tr("Error"),
			tr("Crawler does not contain any data."),
			MessageBoxDialog::InformationIcon, QDialogButtonBox::Ok);

		return;
	}

	const QString path = QFileDialog::getSaveFileName(theApp->mainWindow());
	theApp->crawler()->saveToFile(path);
}

void MainWindow::showOpenFileDialog()
{
	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		theApp->mainWindow()->showMessageBoxDialog(tr("Error"), tr("Cannot open a document while crawler is working!"),
			MessageBoxDialog::CriticalErrorIcon, QDialogButtonBox::Ok);

		return;
	}

	const QString path = QFileDialog::getOpenFileName(theApp->mainWindow());
	theApp->crawler()->loadFromFile(path);
}

void MainWindow::showApplicationSettingsDialog(const QByteArray& settingsPageName)
{
	ApplicationSettingsWidget* applicationSettingsWidget = new ApplicationSettingsWidget(this);
	applicationSettingsWidget->setCurrentPage(settingsPageName);
	applicationSettingsWidget->exec();
	applicationSettingsWidget->deleteLater();
}

void MainWindow::showMessageBoxDialog(const QString& title, 
	const QString& message,
	MessageBoxDialog::Icon icon,
	QDialogButtonBox::StandardButtons buttons)
{
	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog(this);
	messageBoxDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	messageBoxDialog->setWindowTitle(title);
	messageBoxDialog->setMessage(message);
	messageBoxDialog->setIcon(icon);
	messageBoxDialog->setStandardButtons(buttons);
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

	QStatusBar* statusBar = new QStatusBar(this);
	statusBar->addWidget(new CrawlerStatusInfo(statusBar));

	setStatusBar(statusBar);

	m_initialized = true;
}

void MainWindow::createActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// file actions
	actionRegistry.addActionGroup(s_fileActionGroup);

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_openFileAction, QIcon(QStringLiteral(":/images/open-file-icon.png")), tr("Open File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_closeFileAction, tr("Close File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_recentFilesAction, QIcon(QStringLiteral(":/images/actions-document-open-recent-icon.png")), tr("Recent Files"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAction, QIcon(QStringLiteral(":/images/save-icon.png")), tr("Save File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAsAction, QIcon(QStringLiteral(":/images/save-as-icon.png")), tr("Save File As"));

	actionRegistry.addGlobalAction(s_exitProgramAction, tr("Exit"));

	// settings actions
	actionRegistry.addActionGroup(s_settingsActionGroup);

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openSettingsAction, tr("Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCrawlerSettingsAction, QIcon(QStringLiteral(":/images/crawler-settings.png")), tr("Crawler Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLanguageSettingsAction, QIcon(QStringLiteral(":/images/lang-settings.png")), tr("Language Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openPreferencesSettingsAction, QIcon(QStringLiteral(":/images/preferences-settings-icon.png")), tr("Preferences Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLimitsSettingsAction, QIcon(QStringLiteral(":/images/limits-settings.png")), tr("Limit Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openProxySettingsAction, QIcon(QStringLiteral(":/images/proxy-settings.png")), tr("Proxy Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openUserAgentSettingsAction, tr("User Agent Settings"));

	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::crawlerStarted,
		this, [] { ActionRegistry::instance().actionGroup(s_settingsActionGroup)->setDisabled(true); }));

	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::crawlerStopped,
		this, [] { ActionRegistry::instance().actionGroup(s_settingsActionGroup)->setEnabled(true); }));

	VERIFY(connect(actionRegistry.globalAction(s_openSettingsAction), SIGNAL(triggered()), 
		this, SLOT(showApplicationSettingsDialog())));

	VERIFY(connect(actionRegistry.globalAction(s_openCrawlerSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_CrawlerSettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openLanguageSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_LanguageSettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openPreferencesSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_PreferencesSettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openLimitsSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_LimitsSettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openProxySettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_ProxySettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openUserAgentSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_UserAgentSettingsWidget)); }));

	// crawler actions
	actionRegistry.addGlobalAction(s_startCrawlerAction, tr("Start Crawler"));
	actionRegistry.addGlobalAction(s_stopCrawlerAction, tr("Stop Crawler"));
	actionRegistry.addGlobalAction(s_clearCrawledDataAction, tr("Clear Crawled Data"));

	VERIFY(connect(actionRegistry.globalAction(s_startCrawlerAction), SIGNAL(triggered()), theApp, SLOT(startCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_stopCrawlerAction), SIGNAL(triggered()), theApp, SLOT(stopCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_clearCrawledDataAction), SIGNAL(triggered()), theApp, SLOT(clearCrawledData())));
	VERIFY(connect(actionRegistry.globalAction(s_exitProgramAction), SIGNAL(triggered()), theApp, SLOT(quit())));

	// sitemap actions
	actionRegistry.addGlobalAction(s_createXMLSitemapAction, tr("Create XML Sitemap"));

	VERIFY(connect(actionRegistry.globalAction(s_createXMLSitemapAction), SIGNAL(triggered()), this, SLOT(showSitemapCreatorDialog())));
	
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAsAction), SIGNAL(triggered()), this, SLOT(showSaveFileDialog())));
	VERIFY(connect(actionRegistry.globalAction(s_openFileAction), SIGNAL(triggered()), this, SLOT(showOpenFileDialog())));
}

void MainWindow::createAndSetCentralWidget()
{
	QWidget* centralWidget = new QWidget(this);
	DataPagesWidget* dataPagesWidget = new DataPagesWidget(centralWidget);

	PageFactory pageFactory;

	dataPagesWidget->addPage(PageFactory::SiteAuditPage, pageFactory.createPage(PageFactory::SiteAuditPage), tr("Audit Info"), QIcon(), true);
	dataPagesWidget->addPage(PageFactory::AllPagesPage, pageFactory.createPage(PageFactory::AllPagesPage), tr("All Site Pages"));
	dataPagesWidget->addPage(PageFactory::AllResourcesPage, pageFactory.createPage(PageFactory::AllResourcesPage), tr("All Resources"));

	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(dataPagesWidget);

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
	SettingsPageImpl<Ui_UserAgentSettingsWidget>::registerSettingsPage(QIcon(), TYPE_STRING(Ui_UserAgentSettingsWidget), new UserAgentSettingsWidget());
}

}