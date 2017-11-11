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
#include "ui_crawler_settings_widget.h"
#include "ui_proxy_settings_widget.h"
#include "ui_limits_settings_widget.h"
#include "ui_preferences_settings_widget.h"
#include "ui_language_settings_widget.h"

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
	SitemapCreatorWidget* sitemapCreatorWidget = new SitemapCreatorWidget(this);
	sitemapCreatorWidget->exec();
	sitemapCreatorWidget->deleteLater();
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
	setStatusBar(new QStatusBar(this));

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
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCrawlerSettingsAction, tr("Crawler Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLanguageSettingsAction, tr("Language Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openPreferencesSettingsAction, tr("Preferences Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLimitsSettingsAction, tr("Limit Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openProxySettingsAction, tr("Proxy Settings"));

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

}