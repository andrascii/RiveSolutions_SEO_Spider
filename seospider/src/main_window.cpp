#include "application.h"
#include "application_settings_widget.h"
#include "main_window.h"
#include "crawler.h"
#include "content_frame.h"
#include "action_keys.h"
#include "action_registry.h"
#include "menubar.h"
#include "settings_page_impl.h"
#include "site_map_creator_widget.h"
#include "page_factory.h"
#include "settings_page.h"
#include "user_agent_settings_widget.h"
#include "crawler_pause_settings_widget.h"
#include "page_visual_settings_widget.h"
#include "language_settings_widget.h"
#include "proxy_settings_widget.h"
#include "crawler_settings_widget.h"
#include "internet_connection_state_widget.h"
#include "notifications_container_widget.h"
#include "software_branding.h"
#include "svg_renderer.h"
#include "header_controls_container.h"
#include "storage_exporter.h"
#include "helpers.h"
#include "filter_widget.h"
#include "project_file_state_widget.h"
#include "constants.h"
#include "version.h"
#include "recent_files.h"
#include "crawler_progress_bar.h"
#include "license_controls_blocker.h"
#include "ui_limits_settings_widget.h"
#include "ui_preferences_settings_widget.h"
#include "ui_language_settings_widget.h"
#include "ui_user_agent_settings_widget.h"
#include "ui_crawler_pause_settings_widget.h"
#include "ui_company_profile_settings_widget.h"
#include "ui_page_visual_settings_widget.h"


namespace SeoSpider
{

using namespace CrawlerEngine;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_initialized(false)
	, m_systemTrayIcon(new QSystemTrayIcon(theApp->softwareBrandingOptions()->applicationIcon(), this))
{
	setWindowTitle(
		theApp->softwareBrandingOptions()->organizationName() + QStringLiteral(" ") +
		theApp->softwareBrandingOptions()->productName()
	);

	VERIFY(connect(theApp->crawler(), &Crawler::sessionCreated, this, &MainWindow::onCrawlerSessionCreated));
	VERIFY(connect(theApp->crawler(), &Crawler::sessionDestroyed, this, &MainWindow::onCrawlerSessionDestroyed));
	VERIFY(connect(systemTrayIcon(), &QSystemTrayIcon::activated, this, &MainWindow::onSystemTrayIconActivated));
}

void MainWindow::showSitemapCreatorDialog()
{
	SitemapCreatorWidget* titledWindow = new SitemapCreatorWidget(this);
	titledWindow->exec();
	titledWindow->deleteLater();
}

void MainWindow::saveFile()
{
	if (theApp->crawler()->hasCustomSessionName())
	{
		theApp->crawler()->saveFile();
	}
	else
	{
		saveFileAs();
	}
}

void MainWindow::saveFileAs()
{
	const QString path = getSaveFilePath();
	
	if (path.isEmpty())
	{
		return;
	}

	RecentFiles::instance().registerNewRecentFile(path);
	theApp->crawler()->saveToFile(path);
}

void MainWindow::openFile()
{
	if (theApp->crawler()->hasSession())
	{
		showMessageBoxDialog("Open file error", "Unable to open the project file until the existing one is closed!\n"
			"So first you need to press Ctrl+W and then open file.",
			MessageBoxDialog::WarningIcon,
			QDialogButtonBox::Ok);

		return;
	}

	const QString path = QFileDialog::getOpenFileName(theApp->mainWindow(), tr("Open File"), qApp->applicationDirPath(), QString("*" + c_projectFileExtension));

	if (path.isEmpty())
	{
		return;
	}

	RecentFiles::instance().registerNewRecentFile(path);
	theApp->crawler()->loadFromFile(path);
}

void MainWindow::openFile(const QString& filePath)
{
	if (theApp->crawler()->hasSession())
	{
		showMessageBoxDialog("Open file error", "Unable to open the project file until the existing one is closed!\n"
			"So first you need to press Ctrl+W and then open file.",
			MessageBoxDialog::WarningIcon,
			QDialogButtonBox::Ok);

		return;
	}

	if (filePath.isEmpty())
	{
		return;
	}

	RecentFiles::instance().registerNewRecentFile(filePath);
	theApp->crawler()->loadFromFile(filePath);
}

void MainWindow::closeFile()
{
	if (theApp->crawler()->state() != Crawler::StatePending &&
		theApp->crawler()->state() != Crawler::StatePause)
	{
		theApp->mainWindow()->showMessageBoxDialog(
			tr("Warning"),
			tr("Cannot close file while crawler is working."),
			MessageBoxDialog::WarningIcon
		);

		return;
	}

	if (theApp->crawler()->sessionState() == Session::StateUnsaved)
	{
		int answer = theApp->mainWindow()->showMessageBoxDialog(
			tr("Warning"), 
			tr("The project file was not saved, all data will be lost. Do you want to close it anyway?"),
			MessageBoxDialog::WarningIcon
		);

		if (answer == QDialog::Rejected)
		{
			return;
		}
	}

	theApp->crawler()->closeFile();
}

void MainWindow::openRecentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());

	if (action)
	{
		openFile(action->data().toString());
	}
}

void MainWindow::saveFileAndClearData()
{
	const QString path = getSaveFilePath();

	if (path.isEmpty())
	{
		return;
	}

	VERIFY(connect(theApp->crawler(), &Crawler::serializationProcessDone, this, &MainWindow::clearDataOnSerializationDone));

	theApp->crawler()->saveToFile(path);
}

void MainWindow::exportFilterData()
{
	QAction* action = qobject_cast<QAction*>(sender());
	ASSERT(action && "This method must be called using QAction");

	const QVariant objectData = action->data();
	ASSERT(objectData.isValid() && "No data passed");

	std::vector<DCStorageDescription> storages;
	storages.push_back(qvariant_cast<DCStorageDescription>(objectData));

	StorageExporter::exportStorage(theApp->crawler()->sequencedDataCollection(), storages);
}

void MainWindow::onChangeGroupingAuditInfo(QAction* action)
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	actionRegistry.globalAction(s_switchAuditInfoFilterWidgetGroupingAction)->setIcon(action->icon());

	theApp->mainWindow()->showContentFramePage(PageFactory::Page::SiteAuditPage);

	auto page = theApp->mainWindow()->contentFrame()->page(PageFactory::Page::SiteAuditPage);
	auto filterWidget = Common::Helpers::fast_cast<FilterWidget*>(page);

	ASSERT(action->data().type() == QVariant::Bool);
	bool isSortingEnabled = action->data().toBool();

	if (isSortingEnabled)
	{
		filterWidget->enableSortableFilter();
	}
	else
	{
		filterWidget->enablePlainFilter();
	}
}

void MainWindow::showApplicationSettingsDialog(const QByteArray& settingsPageName)
{
	ApplicationSettingsWidget* applicationSettingsWidget = new ApplicationSettingsWidget(this);
	applicationSettingsWidget->setCurrentPage(settingsPageName);
	applicationSettingsWidget->exec();
	applicationSettingsWidget->deleteLater();
}

int MainWindow::showMessageBoxDialog(const QString& title,
	const QString& message,
	MessageBoxDialog::Icon icon,
	QDialogButtonBox::StandardButtons buttons)
{
	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog;
	messageBoxDialog->setWindowTitle(title);
	messageBoxDialog->setMessage(message);
	messageBoxDialog->setIcon(icon);
	messageBoxDialog->setStandardButtons(buttons);
	messageBoxDialog->exec();

	return messageBoxDialog->result();
}

void MainWindow::showContentFramePage(PageFactory::Page page)
{
	emit showPage(page);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	emit resized();
}

void MainWindow::moveEvent(QMoveEvent* event)
{
	QMainWindow::moveEvent(event);

	emit moved();
}

void MainWindow::changeEvent(QEvent* event)
{
	QMainWindow::changeEvent(event);

	if (event->type() == QEvent::WindowStateChange)
	{
		if (static_cast<QWindowStateChangeEvent*>(event)->oldState() == windowState())
		{
			return;
		}

		emit windowStateChanged();
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	const QString warningMessage("Crawler is working. Do you want to close application anyway?");
	const QString descriptionMessage("If no then it will be collapsed to the system tray.");

	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		Qt::WindowStates states = windowState();

		int answer = -1;

		if (states.testFlag(Qt::WindowMinimized))
		{
			answer = showMessageBoxDialog("Closing application",
				warningMessage,
				MessageBoxDialog::WarningIcon,
				QDialogButtonBox::Yes | QDialogButtonBox::No);
		}
		else
		{
			answer = showMessageBoxDialog("Closing application",
				warningMessage % "\n" % descriptionMessage,
				MessageBoxDialog::WarningIcon,
				QDialogButtonBox::Yes | QDialogButtonBox::No);
		}
		
		ASSERT(answer == QDialog::Accepted || answer == QDialog::Rejected);

		if (answer == QDialog::Rejected)
		{
			setWindowState(Qt::WindowMinimized);
			event->ignore();
			return;
		}
	}

	const SoftwareBranding* softwareBrandingOptions = theApp->softwareBrandingOptions();
	QSettings settings(softwareBrandingOptions->organizationName(), softwareBrandingOptions->productName());

	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState(MAINTENANCE));

	QMainWindow::closeEvent(event);
}

void MainWindow::init()
{
	DEBUG_ASSERT(!m_initialized);

	createActions();
	createAndSetCentralWidget();
	registerSettingsPages();
	setWindowIcon(theApp->softwareBrandingOptions()->applicationIcon());
	setMenuBar(new MenuBar(this));

	QStatusBar* statusBar = new QStatusBar(this);
	statusBar->addPermanentWidget(new NotificationsContainerWidget(statusBar));
	statusBar->addPermanentWidget(new InternetConnectionStateWidget(statusBar));
	statusBar->addWidget(new ProjectFileStateWidget(statusBar));
	statusBar->addWidget(new CrawlerProgressBar(statusBar));
	setStatusBar(statusBar);

	new LicenseControlsBlocker(this);

	loadState();

	initSystemTrayIconMenu();
	systemTrayIcon()->show();

	m_initialized = true;
}

void MainWindow::createActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// file actions
	actionRegistry.addActionGroup(s_fileActionGroup);
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_openFileAction, QIcon(QStringLiteral(":/images/open-file-icon.png")), tr("Open File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_closeFileAction, tr("Close File"));

	QAction* recentFilesAction = RecentFiles::instance().subMenuAction();
	recentFilesAction->setIcon(QIcon(QStringLiteral(":/images/actions-document-open-recent-icon.png")));
	recentFilesAction->setText(tr("Recent Files"));

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_recentFilesAction, recentFilesAction);
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAction, QIcon(QStringLiteral(":/images/save-icon.png")), tr("Save File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAsAction, QIcon(QStringLiteral(":/images/save-as-icon.png")), tr("Save File As"));
	actionRegistry.addGlobalAction(s_exitProgramAction, tr("Exit"));
	actionRegistry.addGlobalAction(s_saveFileAndClearDataAction, tr("Save To File And Clear Data"));

	actionRegistry.globalAction(s_saveFileAction)->setDisabled(true);
	actionRegistry.globalAction(s_closeFileAction)->setDisabled(true);

	// settings actions
	actionRegistry.addActionGroup(s_settingsActionGroup);
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCrawlerSettingsAction, QIcon(QStringLiteral(":/images/crawler-settings.png")), tr("Crawler Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLanguageSettingsAction, QIcon(QStringLiteral(":/images/lang-settings.png")), tr("Language Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openPreferencesSettingsAction, QIcon(QStringLiteral(":/images/preferences-settings-icon.png")), tr("Preferences Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLimitsSettingsAction, QIcon(QStringLiteral(":/images/limits-settings.png")), tr("Limit Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openProxySettingsAction, QIcon(QStringLiteral(":/images/proxy-settings.png")), tr("Proxy Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openUserAgentSettingsAction, QIcon(QStringLiteral(":/images/user-agent.png")), tr("User Agent Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCrawlerPauseTimerSettingsAction, QIcon(QStringLiteral(":/images/crawler-pause.png")), tr("Crawler Pause Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCompanyProfileSettingsAction, QIcon(QStringLiteral(":/images/company-profile.png")), tr("Company Profile Settings"));
	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openPageVisualSettingsAction, QIcon(QStringLiteral(":/images/color.png")), tr("Page Visual Settings"));

	const auto settingsActionsAvailability = [](int state)
	{
		const auto actionsAvailabilitySetter = [](bool value)
		{
			ActionRegistry::instance().actionGroup(s_settingsActionGroup)->setEnabled(value);
			ActionRegistry::instance().globalAction(s_openFileAction)->setEnabled(value);
		};

		if (state == Crawler::StatePreChecking || state == Crawler::StateWorking)
		{
			actionsAvailabilitySetter(false);
		}

		if (state == Crawler::StatePending)
		{
			actionsAvailabilitySetter(true);
		}
	};

	VERIFY(connect(theApp->crawler(), &Crawler::stateChanged, this, settingsActionsAvailability));

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

	VERIFY(connect(actionRegistry.globalAction(s_openCrawlerPauseTimerSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_CrawlerPauseSettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openCompanyProfileSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_CompanyProfileSettingsWidget)); }));

	VERIFY(connect(actionRegistry.globalAction(s_openPageVisualSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_PageVisualSettingsWidget)); }));

	// crawler actions
	actionRegistry.addGlobalAction(s_startCrawlerAction, tr("Start Crawler"));
	actionRegistry.addGlobalAction(s_stopCrawlerAction, tr("Stop Crawler"));
	actionRegistry.addGlobalAction(s_clearCrawledDataAction, tr("Clear Crawled Data"));

	VERIFY(connect(actionRegistry.globalAction(s_startCrawlerAction), SIGNAL(triggered()), theApp, SLOT(startCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_stopCrawlerAction), SIGNAL(triggered()), theApp, SLOT(stopCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_clearCrawledDataAction), SIGNAL(triggered()), theApp, SLOT(clearCrawledData())));
	VERIFY(connect(actionRegistry.globalAction(s_exitProgramAction), SIGNAL(triggered()), theApp, SLOT(closeAllWindows())));

	actionRegistry.globalAction(s_exitProgramAction)->setShortcut(QKeySequence("Alt+F4"));

	// sitemap actions
	actionRegistry.addGlobalAction(s_createXMLSitemapAction, tr("Create XML Sitemap"));

	VERIFY(connect(actionRegistry.globalAction(s_createXMLSitemapAction), SIGNAL(triggered()), this, SLOT(showSitemapCreatorDialog())));
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAsAction), SIGNAL(triggered()), this, SLOT(saveFileAs())));
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAction), SIGNAL(triggered()), this, SLOT(saveFile())));
	VERIFY(connect(actionRegistry.globalAction(s_openFileAction), SIGNAL(triggered()), this, SLOT(openFile())));
	VERIFY(connect(actionRegistry.globalAction(s_closeFileAction), SIGNAL(triggered()), this, SLOT(closeFile())));
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAndClearDataAction), SIGNAL(triggered()), this, SLOT(saveFileAndClearData())));

	actionRegistry.globalAction(s_openFileAction)->setShortcut(QKeySequence("Ctrl+O"));
	actionRegistry.globalAction(s_saveFileAction)->setShortcut(QKeySequence("Ctrl+S"));
	actionRegistry.globalAction(s_saveFileAsAction)->setShortcut(QKeySequence("Ctrl+Alt+S"));
	actionRegistry.globalAction(s_closeFileAction)->setShortcut(QKeySequence("Ctrl+W")); 

	createHeaderPageDependentActions();
}

void MainWindow::createHeaderPageDependentActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// switch grouping action
	QAction* switchAuditInfoFilterWidgetGroupingAction = actionRegistry.addGlobalAction(s_switchAuditInfoFilterWidgetGroupingAction,
		SvgRenderer::render(QStringLiteral(":/images/group-by-category.svg"), 20, 20), tr("Change filters grouping"));

	QToolButton* menuButton = qobject_cast<QToolButton*>(HeaderToolButtonCreator::createControl(switchAuditInfoFilterWidgetGroupingAction));

	ASSERT(menuButton);

	QMenu* groupingMenu = new QMenu(menuButton);

	QAction* groupByCategoryAction = new QAction(SvgRenderer::render(":/images/group-by-category.svg", 20, 20), "Group filters by category");
	QAction* groupByErrorLevelAction = new QAction(SvgRenderer::render(":/images/group-by-level.svg", 20, 20), "Group filters by error level");

	QVariant enableSortingData(true);
	QVariant disableSortingData(false);

	groupByErrorLevelAction->setData(enableSortingData);
	groupByCategoryAction->setData(disableSortingData);

	groupingMenu->addAction(groupByCategoryAction);
	groupingMenu->addAction(groupByErrorLevelAction);

	menuButton->setMenu(groupingMenu);
	menuButton->setPopupMode(QToolButton::InstantPopup);

	theApp->headerControlsContainer()->addWidget(menuButton, PageFactory::Page::SiteAuditPage);

	VERIFY(connect(groupingMenu, SIGNAL(triggered(QAction*)), this, SLOT(onChangeGroupingAuditInfo(QAction*))));

	//////////////////////////////////////////////////////////////////////////

	// export actions
	QAction* exportFilterDataAuditPageAction = actionRegistry.addGlobalAction(s_exportFilterDataAuditPageAction,
		SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20), tr("Export selected filter data to .xlsx file"));

	QAction* exportFilterDataAllResourcesAction = actionRegistry.addGlobalAction(s_exportFilterDataAllResourcesPageAction,
		SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20), tr("Export selected filter data to .xlsx file"));

	exportFilterDataAuditPageAction->setDisabled(true);
	exportFilterDataAllResourcesAction->setDisabled(true);

	theApp->headerControlsContainer()->addAction(exportFilterDataAllResourcesAction, PageFactory::Page::AllResourcesPage);
	theApp->headerControlsContainer()->addAction(exportFilterDataAuditPageAction, PageFactory::Page::SiteAuditPage);

	VERIFY(connect(exportFilterDataAllResourcesAction, &QAction::triggered, this, &MainWindow::exportFilterData));
	VERIFY(connect(exportFilterDataAuditPageAction, &QAction::triggered, this, &MainWindow::exportFilterData));
}

void MainWindow::createAndSetCentralWidget()
{
	QWidget* centralWidget = new QWidget(this);
	m_contentFrame = new ContentFrame(centralWidget);

	PageFactory pageFactory;

	m_contentFrame->addPage(PageFactory::SiteAuditPage, pageFactory.createPage(PageFactory::SiteAuditPage),
		tr("Audit Info"), pageFactory.createPageIcon(PageFactory::SiteAuditPage), true);

	m_contentFrame->addPage(PageFactory::AllPagesPage, pageFactory.createPage(PageFactory::AllPagesPage),
		tr("All Site Pages"), pageFactory.createPageIcon(PageFactory::AllPagesPage));

	m_contentFrame->addPage(PageFactory::AllResourcesPage, pageFactory.createPage(PageFactory::AllResourcesPage),
		tr("All Resources"), pageFactory.createPageIcon(PageFactory::AllResourcesPage));

	m_contentFrame->addPage(PageFactory::AuditReportPage, pageFactory.createPage(PageFactory::AuditReportPage),
		tr("Audit Report"), pageFactory.createPageIcon(PageFactory::AuditReportPage));

	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(m_contentFrame);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
}

void MainWindow::registerSettingsPages() const
{
	SettingsPageImpl<Ui_CrawlerSettingsWidget>::registerSettingsPage(QIcon(":/images/crawler-settings.png"), TYPE_STRING(Ui_CrawlerSettingsWidget), new CrawlerSettingsWidget);
	SettingsPageImpl<Ui_ProxySettingsWidget>::registerSettingsPage(QIcon(":/images/proxy-settings.png"), TYPE_STRING(Ui_ProxySettingsWidget), new ProxySettingsWidget);
	SettingsPageImpl<Ui_LimitsSettingsWidget>::registerSettingsPage(QIcon(":/images/limits-settings.png"), TYPE_STRING(Ui_LimitsSettingsWidget));
	SettingsPageImpl<Ui_PreferencesSettingsWidget>::registerSettingsPage(QIcon(":/images/preferences-settings-icon.png"), TYPE_STRING(Ui_PreferencesSettingsWidget));
	SettingsPageImpl<Ui_LanguageSettingsWidget>::registerSettingsPage(QIcon(":/images/lang-settings.png"), TYPE_STRING(Ui_LanguageSettingsWidget), new LanguageSettingsWidget);
	SettingsPageImpl<Ui_UserAgentSettingsWidget>::registerSettingsPage(QIcon(":/images/user-agent.png"), TYPE_STRING(Ui_UserAgentSettingsWidget), new UserAgentSettingsWidget);
	SettingsPageImpl<Ui_CrawlerPauseSettingsWidget>::registerSettingsPage(QIcon(":/images/crawler-pause.png"), TYPE_STRING(Ui_CrawlerPauseSettingsWidget), new CrawlerPauseSettingsWidget);
	SettingsPageImpl<Ui_CompanyProfileSettingsWidget>::registerSettingsPage(QIcon(":/images/company-profile.png"), TYPE_STRING(Ui_CompanyProfileSettingsWidget));

#ifndef PRODUCTION
	SettingsPageImpl<Ui_PageVisualSettingsWidget>::registerSettingsPage(QIcon(":/images/color.png"), TYPE_STRING(Ui_PageVisualSettingsWidget), new PageVisualSettingsWidget);
#endif
}

QString MainWindow::getSaveFilePath() const
{
	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		theApp->mainWindow()->showMessageBoxDialog(tr("Error"), tr("Cannot save the document while crawler is working!"),
			MessageBoxDialog::CriticalErrorIcon, QDialogButtonBox::Ok);

		return QString();
	}

	if (theApp->crawler()->hasNoData())
	{
		theApp->mainWindow()->showMessageBoxDialog(tr("Error"),
			tr("Crawler does not contain any data."),
			MessageBoxDialog::InformationIcon, QDialogButtonBox::Ok);

		return QString();
	}

	const QString path = QFileDialog::getSaveFileName(theApp->mainWindow(), tr("Save File"), qApp->applicationDirPath() + "/" + theApp->crawler()->currentCrawledUrl(), QString("*" + c_projectFileExtension));

	return path;
}

void MainWindow::loadState()
{
	const SoftwareBranding* softwareBrandingOptions = theApp->softwareBrandingOptions();
	QSettings settings(softwareBrandingOptions->organizationName(), softwareBrandingOptions->productName());

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::clearDataOnSerializationDone()
{
	VERIFY(disconnect(theApp->crawler(), &Crawler::serializationProcessDone, this, &MainWindow::clearDataOnSerializationDone));
	theApp->crawler()->clearData();
}

void MainWindow::initSystemTrayIconMenu()
{
	QMenu* menu = new QMenu;
	menu->addAction(ActionRegistry::instance().globalAction(s_startCrawlerAction));
	menu->addAction(ActionRegistry::instance().globalAction(s_stopCrawlerAction));
	menu->addSeparator();
	menu->addAction(ActionRegistry::instance().globalAction(s_exitProgramAction));
	systemTrayIcon()->setContextMenu(menu);
}

void MainWindow::onCrawlerSessionCreated()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QAction* saveFileAction = actionRegistry.globalAction(s_saveFileAction);
	QAction* saveFileAsAction = actionRegistry.globalAction(s_saveFileAsAction);
	QAction* closeFileAction = actionRegistry.globalAction(s_closeFileAction);

	saveFileAction->setEnabled(true);
	saveFileAsAction->setEnabled(true);
	closeFileAction->setEnabled(true);
}

void MainWindow::onCrawlerSessionDestroyed()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QAction* saveFileAction = actionRegistry.globalAction(s_saveFileAction);
	QAction* saveFileAsAction = actionRegistry.globalAction(s_saveFileAsAction);
	QAction* closeFileAction = actionRegistry.globalAction(s_closeFileAction);

	saveFileAction->setEnabled(false);
	saveFileAsAction->setEnabled(false);
	closeFileAction->setEnabled(false);
}

void MainWindow::onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick)
	{
		Qt::WindowStates states = windowState();
		states.setFlag(Qt::WindowMinimized, false);

		setWindowState(states);
		activateWindow();
	}
}

ContentFrame* MainWindow::contentFrame() const noexcept
{
	return m_contentFrame;
}

QSystemTrayIcon* MainWindow::systemTrayIcon() const noexcept
{
	return m_systemTrayIcon;
}

}
