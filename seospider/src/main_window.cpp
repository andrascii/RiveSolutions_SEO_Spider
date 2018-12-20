#include "application.h"
#include "application_settings_dialog.h"
#include "main_window.h"
#include "crawler.h"
#include "content_frame.h"
#include "action_keys.h"
#include "action_registry.h"
#include "menubar.h"
#include "settings_page_impl.h"
#include "site_map_creator_dialog.h"
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
#include "project_file_state_widget.h"
#include "constants.h"
#include "version.h"
#include "recent_files.h"
#include "crawler_progress_bar.h"
#include "license_controls_blocker.h"
#include "register_product_dialog.h"
#include "license_state_observer.h"
#include "common_constants.h"
#include "feedback_dialog.h"
#include "update_checker.h"
#include "update_loader_dialog.h"
#include "shaded_overlay.h"
#include "message_box_dialog.h"
#include "site_audit_page.h"
#include "all_pages_page.h"
#include "all_resources_page.h"
#include "audit_report_page.h"
#include "yandex_metrica_settings_widget.h"
#include "ui_limits_settings_widget.h"
#include "ui_preferences_settings_widget.h"
#include "ui_language_settings_widget.h"
#include "ui_user_agent_settings_widget.h"
#include "ui_crawler_pause_settings_widget.h"
#include "ui_company_profile_settings_widget.h"
#include "ui_page_visual_settings_widget.h"
#include "custom_uri_channel.h"
#include "statistic_counter.h"
#include "limits_settings_widget.h"

namespace
{

const QString s_helpPage = "https://rivesolutions.com/faq/";

}

namespace SeoSpider
{

using namespace CrawlerEngine;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_initialized(false)
	, m_systemTrayIcon(new QSystemTrayIcon(theApp->softwareBrandingOptions()->applicationIcon(), this))
	, m_updateChecker(new UpdateChecker(this))
	, m_shadedOverlay(new ShadedOverlay(this))
	, m_applicationSettingsDialog(nullptr)
	, m_delayedHideShadedOverlayTimer(new QTimer(this))
{
	qRegisterMetaType<Version>("Version");

	m_delayedHideShadedOverlayTimer->setInterval(100);
	VERIFY(connect(m_delayedHideShadedOverlayTimer, &QTimer::timeout, this, &MainWindow::delayedHideShadedOverlay));

	setWindowTitle(
		theApp->softwareBrandingOptions()->organizationName() + QStringLiteral(" ") +
		theApp->softwareBrandingOptions()->productName()
	);

	VERIFY(connect(theApp->crawler(), &Crawler::sessionCreated, this, &MainWindow::onCrawlerSessionCreated));
	VERIFY(connect(theApp->crawler(), &Crawler::sessionDestroyed, this, &MainWindow::onCrawlerSessionDestroyed));
#ifdef USE_CUSTOM_URI_CHANNEL
	VERIFY(connect(CustomUrlChannel::instance(), &CustomUrlChannel::uriReceived, this, &MainWindow::onCustomUrlReceived, Qt::QueuedConnection));
#endif
	VERIFY(connect(systemTrayIcon(), &QSystemTrayIcon::activated, this, &MainWindow::onSystemTrayIconActivated));
	VERIFY(connect(m_updateChecker->qobject(), SIGNAL(updateExists()), SLOT(onAboutUpdateExists())));
	VERIFY(connect(m_updateChecker->qobject(), SIGNAL(updateIsNotExists()), SLOT(onAboutUpdateIsNotExists())));
}

void MainWindow::showSitemapCreatorDialog()
{
	SitemapCreatorDialog* titledWindow = new SitemapCreatorDialog(this);
	titledWindow->exec();
	titledWindow->deleteLater();
}

void MainWindow::saveFile()
{
	if (theApp->crawler()->hasCustomSessionName())
	{
		if (theApp->crawler()->state() == Crawler::StateWorking)
		{
			ASSERT(ServiceLocator::instance()->isRegistered<INotificationService>());
			INotificationService* notificationService = ServiceLocator::instance()->service<INotificationService>();

			notificationService->error(
				tr("Operation failed"),
				tr("Cannot save file while crawler is active, \nplease stop crawling and try again")
			);

			return;
		}

		theApp->crawler()->saveFile();
	}
	else
	{
		saveFileAs();
	}

	StatisticCounter saveToFileCounter(QString("SaveProjectToFileCounter"));
	saveToFileCounter.increment();
}

void MainWindow::saveFileAs()
{
	QString path = getSaveFilePath();

	if (path.isEmpty())
	{
		return;
	}

	if(!path.contains(c_projectFileExtension))
	{
		path.append(c_projectFileExtension);
	}

	RecentFiles::instance().registerNewRecentFile(path);
	theApp->crawler()->saveToFile(path);
}

void MainWindow::openFile()
{
	if (theApp->crawler()->hasSession())
	{
		Dialog::showMessageBoxDialog(tr("Open file error"),
			tr("Unable to open the project file until the existing one is closed! "
			"So first you need to press Ctrl+W and then open file."),
			QDialogButtonBox::Ok
		);

		return;
	}

	const QString path = QFileDialog::getOpenFileName(theApp->mainWindow(), tr("Open File"), qApp->applicationDirPath(), QString("*" + c_projectFileExtension));

	if (path.isEmpty())
	{
		return;
	}

	StatisticCounter openFromFileCounter(QString("OpenProjectFromFileCounter"));
	openFromFileCounter.increment();

	RecentFiles::instance().registerNewRecentFile(path);
	theApp->crawler()->loadFromFile(path);
}

void MainWindow::openFile(const QString& filePath)
{
	if (theApp->crawler()->hasSession())
	{
		Dialog::showMessageBoxDialog(tr("Open file error"),
			tr("Unable to open the project file until the existing one is closed! "
			"So first you need to press Ctrl+W and then open file."),
			QDialogButtonBox::Ok
		);

		return;
	}

	if (filePath.isEmpty())
	{
		return;
	}

	StatisticCounter openFromFileCounter(QString("OpenProjectFromFileCounter"));
	openFromFileCounter.increment();

	RecentFiles::instance().registerNewRecentFile(filePath);
	theApp->crawler()->loadFromFile(filePath);
}

void MainWindow::closeFile()
{
	if (theApp->crawler()->state() != Crawler::StatePending &&
		theApp->crawler()->state() != Crawler::StatePause)
	{
		Dialog::showMessageBoxDialog(
			tr("Warning"),
			tr("Cannot close file while crawler is working."),
			QDialogButtonBox::Ok
		);

		return;
	}

	if (theApp->crawler()->sessionState() == Session::StateUnsaved)
	{
		int answer = Dialog::showMessageBoxDialog(
			tr("Warning"),
			tr("The project file was not saved, all data will be lost. Do you want to close it anyway?"),
			QDialogButtonBox::Yes | QDialogButtonBox::No
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

void MainWindow::showApplicationSettingsDialog(const QByteArray& settingsPageName)
{
	if (m_applicationSettingsDialog == nullptr)
	{
		m_applicationSettingsDialog = new ApplicationSettingsDialog(this);
	}

	m_applicationSettingsDialog->setCurrentPage(settingsPageName);
	m_applicationSettingsDialog->exec();
}

void MainWindow::onCrawlingFinished() const
{
	DEBUG_ASSERT(ServiceLocator::instance()->isRegistered<ILicenseStateObserver>());
	ILicenseStateObserver* licenseService = ServiceLocator::instance()->service<ILicenseStateObserver>();

	if (!licenseService)
	{
		return;
	}

	if (licenseService->isTrialLicense() &&
		theApp->crawler()->scannedPagesCount() >= Common::c_maxTrialLicenseCrawlingLinksCount)
	{
		Dialog::showMessageBoxDialog(
			tr("Trial license"),
			tr("This option is not available in the trial version."),
			QDialogButtonBox::Ok
		);
	}
}

void MainWindow::showShadedOverlay()
{
	m_shadedOverlay->setVisible(true);
	if (m_delayedHideShadedOverlayTimer->isActive())
	{
		m_delayedHideShadedOverlayTimer->stop();
	}
}

void MainWindow::hideShadedOverlay()
{
	if (m_delayedHideShadedOverlayTimer->isActive())
	{
		m_delayedHideShadedOverlayTimer->stop();
	}

	m_delayedHideShadedOverlayTimer->start();
}

void MainWindow::delayedHideShadedOverlay()
{
	m_shadedOverlay->setVisible(false);
}

void MainWindow::onCustomUrlReceived()
{
	setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
	raise();  // for MacOS
	activateWindow(); // for Windows
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	m_shadedOverlay->resize(size());

	emit resized();
}

void MainWindow::moveEvent(QMoveEvent* event)
{
	QMainWindow::moveEvent(event);

	m_shadedOverlay->move(mapToGlobal(rect().topLeft()));

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
	const QString warningMessage(tr("Crawler is working. Do you want to close application anyway?"));
	const QString descriptionMessage(tr("If no then it will be collapsed to the system tray."));

	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		Qt::WindowStates states = windowState();

		int answer = -1;

		MessageBoxDialog* messageBoxDialog = new MessageBoxDialog;
		messageBoxDialog->setMessage(warningMessage + " " + descriptionMessage);
		messageBoxDialog->addButton("Close", QDialogButtonBox::ButtonRole::AcceptRole);
		messageBoxDialog->addButton("Collapse", QDialogButtonBox::ButtonRole::RejectRole);
		messageBoxDialog->exec();

		answer = messageBoxDialog->result();

		ASSERT(answer == QDialog::Accepted || answer == QDialog::Rejected);

		if (answer == QDialog::Rejected)
		{
			hide();
			event->ignore();
			return;
		}
	}

	const SoftwareBranding* softwareBrandingOptions = theApp->softwareBrandingOptions();
	QSettings settings(softwareBrandingOptions->organizationName(), softwareBrandingOptions->productName());

	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState(MAINTENANCE));

	if(systemTrayIcon()->isVisible())
	{
		systemTrayIcon()->hide();
	}

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

	m_updateChecker->check();

	m_initialized = true;
}

void MainWindow::createActions()
{
	ActionRegistry::instance().addGlobalAction(s_createXMLSitemapAction, tr("Create XML Sitemap"));

	initFileActions();
	initSettingsActions();
	initHelpActions();
	initCrawlerActions();
}

void MainWindow::createAndSetCentralWidget()
{
	QWidget* centralWidget = new QWidget(this);
	m_contentFrame = new ContentFrame(centralWidget);

	m_contentFrame->addPage(new SiteAuditPage, true);
	m_contentFrame->addPage(new AllPagesPage);
	m_contentFrame->addPage(new AllResourcesPage);
	m_contentFrame->addPage(new AuditReportPage);

	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(m_contentFrame);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
}

void MainWindow::registerSettingsPages() const
{
	SettingsPageImpl<Ui_CrawlerSettingsWidget>::registerSettingsPage(
		QIcon(":/images/crawler-settings.png"),
		TYPE_STRING(Ui_CrawlerSettingsWidget),
		new CrawlerSettingsWidget);

	SettingsPageImpl<Ui_ProxySettingsWidget>::registerSettingsPage(
		QIcon(":/images/proxy-settings.png"),
		TYPE_STRING(Ui_ProxySettingsWidget),
		new ProxySettingsWidget);

	SettingsPageImpl<Ui_LimitsSettingsWidget>::registerSettingsPage(
		QIcon(":/images/limits-settings.png"),
		TYPE_STRING(Ui_LimitsSettingsWidget),
		new LimitsSettingsWidget);

	SettingsPageImpl<Ui_PreferencesSettingsWidget>::registerSettingsPage(
		QIcon(":/images/preferences-settings-icon.png"),
		TYPE_STRING(Ui_PreferencesSettingsWidget));

	SettingsPageImpl<Ui_LanguageSettingsWidget>::registerSettingsPage(
		QIcon(":/images/lang-settings.png"),
		TYPE_STRING(Ui_LanguageSettingsWidget),
		new LanguageSettingsWidget);

	SettingsPageImpl<Ui_UserAgentSettingsWidget>::registerSettingsPage(
		QIcon(":/images/user-agent.png"),
		TYPE_STRING(Ui_UserAgentSettingsWidget),
		new UserAgentSettingsWidget);

	SettingsPageImpl<Ui_CrawlerPauseSettingsWidget>::registerSettingsPage(
		QIcon(":/images/crawler-pause.png"),
		TYPE_STRING(Ui_CrawlerPauseSettingsWidget),
		new CrawlerPauseSettingsWidget);

	SettingsPageImpl<Ui_CompanyProfileSettingsWidget>::registerSettingsPage(
		QIcon(":/images/company-profile.png"),
		TYPE_STRING(Ui_CompanyProfileSettingsWidget));

	SettingsPageImpl<Ui_CompanyProfileSettingsWidget>::registerSettingsPage(
		SvgRenderer::render(":/images/yandex.svg", 10, 10),
		TYPE_STRING(Ui_YandexMetricaSettingsWidget),
		new YandexMetricaSettingsWidget);

#ifndef PRODUCTION
	SettingsPageImpl<Ui_PageVisualSettingsWidget>::registerSettingsPage(
		QIcon(":/images/color.png"),
		TYPE_STRING(Ui_PageVisualSettingsWidget),
		new PageVisualSettingsWidget);
#endif
}

QString MainWindow::getSaveFilePath() const
{
	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		Dialog::showMessageBoxDialog(tr("Error"),
			tr("Cannot save the document while crawler is working!"),
			QDialogButtonBox::Ok
		);

		return QString();
	}

	if (theApp->crawler()->hasNoData())
	{
		Dialog::showMessageBoxDialog(tr("Error"),
			tr("Crawler does not contain any data."),
			QDialogButtonBox::Ok
		);

		return QString();
	}

	const QString path = QFileDialog::getSaveFileName(
		theApp->mainWindow(),
		tr("Save File"),
		qApp->applicationDirPath() + "/" + theApp->crawler()->currentCrawledUrl(),
		QString("*" + c_projectFileExtension)
	);

	return path;
}

void MainWindow::initHelpActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	const SoftwareBranding* softwareBranding = theApp->softwareBrandingOptions();

	actionRegistry.addGlobalAction(s_showHelpAction, tr("Show Help"));
	actionRegistry.addGlobalAction(s_sendFeedbackAction, tr("Send Feedback"));
#ifdef CHECK_LICENSE
	actionRegistry.addGlobalAction(s_registerProductAction, tr("Register Product"));
#endif
	actionRegistry.addGlobalAction(s_checkForUpdatesAction, tr("Check for Updates"));
	actionRegistry.addGlobalAction(s_aboutProductAction, tr("About") + " " + softwareBranding->organizationName() + " " + softwareBranding->productName());

	VERIFY(connect(actionRegistry.globalAction(s_sendFeedbackAction), &QAction::triggered,
		this, [this] { showFeedbackDialog(); }));
#ifdef CHECK_LICENSE
	VERIFY(connect(actionRegistry.globalAction(s_registerProductAction), &QAction::triggered,
		this, [this] { showRegisterProductDialog(); }));
#endif

	VERIFY(connect(actionRegistry.globalAction(s_checkForUpdatesAction), &QAction::triggered,
		this, [this] { m_updateChecker->check(); }));

	VERIFY(connect(actionRegistry.globalAction(s_showHelpAction), SIGNAL(triggered()),
		this, SLOT(openHelpPage())));
}

void MainWindow::initFileActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

#ifdef SUPPORT_SERIALIZATION
	actionRegistry.addActionGroup(s_fileActionGroup);
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_openFileAction, QIcon(QStringLiteral(":/images/open-file-icon.png")), tr("Open File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_closeFileAction, tr("Close File"));

	QAction* recentFilesAction = RecentFiles::instance().subMenuAction();
	recentFilesAction->setIcon(QIcon(QStringLiteral(":/images/actions-document-open-recent-icon.png")));
	recentFilesAction->setText(tr("Recent Files"));

	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_recentFilesAction, recentFilesAction);
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAction, QIcon(QStringLiteral(":/images/save-icon.png")), tr("Save File"));
	actionRegistry.addActionToActionGroup(s_fileActionGroup, s_saveFileAsAction, QIcon(QStringLiteral(":/images/save-as-icon.png")), tr("Save File As"));
#endif
	actionRegistry.addGlobalAction(s_exitProgramAction, tr("Exit"));
#ifdef SUPPORT_SERIALIZATION
	actionRegistry.addGlobalAction(s_saveFileAndClearDataAction, tr("Save To File And Clear Data"));

	actionRegistry.globalAction(s_saveFileAction)->setDisabled(true);
	actionRegistry.globalAction(s_closeFileAction)->setDisabled(true);
#endif

	VERIFY(connect(actionRegistry.globalAction(s_createXMLSitemapAction), SIGNAL(triggered()), this, SLOT(showSitemapCreatorDialog())));
#ifdef SUPPORT_SERIALIZATION
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAsAction), SIGNAL(triggered()), this, SLOT(saveFileAs())));
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAction), SIGNAL(triggered()), this, SLOT(saveFile())));
	VERIFY(connect(actionRegistry.globalAction(s_openFileAction), SIGNAL(triggered()), this, SLOT(openFile())));
	VERIFY(connect(actionRegistry.globalAction(s_closeFileAction), SIGNAL(triggered()), this, SLOT(closeFile())));
	VERIFY(connect(actionRegistry.globalAction(s_saveFileAndClearDataAction), SIGNAL(triggered()), this, SLOT(saveFileAndClearData())));

	actionRegistry.globalAction(s_openFileAction)->setShortcut(QKeySequence("Ctrl+O"));
	actionRegistry.globalAction(s_saveFileAction)->setShortcut(QKeySequence("Ctrl+S"));
	actionRegistry.globalAction(s_saveFileAsAction)->setShortcut(QKeySequence("Ctrl+Alt+S"));
	actionRegistry.globalAction(s_closeFileAction)->setShortcut(QKeySequence("Ctrl+W"));
#endif
}

void MainWindow::initSettingsActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	actionRegistry.addActionGroup(s_settingsActionGroup);

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCrawlerSettingsAction,
		QIcon(QStringLiteral(":/images/crawler-settings.png")), tr("Crawler Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLanguageSettingsAction,
		QIcon(QStringLiteral(":/images/lang-settings.png")), tr("Language Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openPreferencesSettingsAction,
		QIcon(QStringLiteral(":/images/preferences-settings-icon.png")), tr("Preferences Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openLimitsSettingsAction,
		QIcon(QStringLiteral(":/images/limits-settings.png")), tr("Limit Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openProxySettingsAction,
		QIcon(QStringLiteral(":/images/proxy-settings.png")), tr("Proxy Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openUserAgentSettingsAction,
		QIcon(QStringLiteral(":/images/user-agent.png")), tr("User-Agent Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCrawlerPauseTimerSettingsAction,
		QIcon(QStringLiteral(":/images/crawler-pause.png")), tr("Crawler Pause Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openCompanyProfileSettingsAction,
		QIcon(QStringLiteral(":/images/company-profile.png")), tr("Company Profile Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_openPageVisualSettingsAction,
		QIcon(QStringLiteral(":/images/color.png")), tr("Page Visual Settings"));

	actionRegistry.addActionToActionGroup(s_settingsActionGroup, s_yandexMetricaSettingsAction,
		SvgRenderer::render(":/images/yandex.svg", 10, 10), tr("Yandex Metrica Settings"));

	const auto settingsActionsAvailability = [](int state)
	{
		const auto actionsAvailabilitySetter = [](bool value)
		{
			ActionRegistry::instance().actionGroup(s_settingsActionGroup)->setEnabled(value);
#ifdef SUPPORT_SERIALIZATION
			ActionRegistry::instance().globalAction(s_openFileAction)->setEnabled(value);
#endif
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

	VERIFY(connect(actionRegistry.globalAction(s_yandexMetricaSettingsAction), &QAction::triggered,
		this, [this] { showApplicationSettingsDialog(TYPE_STRING(Ui_YandexMetricaSettingsWidget)); }));
}

void MainWindow::initCrawlerActions()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	actionRegistry.addGlobalAction(s_startCrawlerAction, tr("Start Crawler"));
	actionRegistry.addGlobalAction(s_stopCrawlerAction, tr("Stop Crawler"));
	actionRegistry.addGlobalAction(s_clearCrawledDataAction, tr("Clear Crawled Data"));

	VERIFY(connect(actionRegistry.globalAction(s_startCrawlerAction), SIGNAL(triggered()), theApp, SLOT(startCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_stopCrawlerAction), SIGNAL(triggered()), theApp, SLOT(stopCrawler())));
	VERIFY(connect(actionRegistry.globalAction(s_clearCrawledDataAction), SIGNAL(triggered()), theApp, SLOT(clearCrawledData())));
	VERIFY(connect(actionRegistry.globalAction(s_exitProgramAction), SIGNAL(triggered()), theApp, SLOT(closeAllWindows())));

	actionRegistry.globalAction(s_exitProgramAction)->setShortcut(QKeySequence("Alt+F4"));
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
#ifdef SUPPORT_SERIALIZATION
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QAction* saveFileAction = actionRegistry.globalAction(s_saveFileAction);
	QAction* saveFileAsAction = actionRegistry.globalAction(s_saveFileAsAction);
	QAction* closeFileAction = actionRegistry.globalAction(s_closeFileAction);

	saveFileAction->setEnabled(true);
	saveFileAsAction->setEnabled(true);
	closeFileAction->setEnabled(true);
#endif
}

void MainWindow::onCrawlerSessionDestroyed()
{
#ifdef SUPPORT_SERIALIZATION
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QAction* saveFileAction = actionRegistry.globalAction(s_saveFileAction);
	QAction* saveFileAsAction = actionRegistry.globalAction(s_saveFileAsAction);
	QAction* closeFileAction = actionRegistry.globalAction(s_closeFileAction);

	saveFileAction->setEnabled(false);
	saveFileAsAction->setEnabled(false);
	closeFileAction->setEnabled(false);
#endif
}

void MainWindow::onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick && !isVisible())
	{
		show();

		activateWindow();
	}
}

void MainWindow::showRegisterProductDialog()
{
	RegisterProductDialog* registerProductDialog = new RegisterProductDialog(this);
	registerProductDialog->open();
}

void MainWindow::showFeedbackDialog()
{
	FeedbackDialog* feddbackDialog = new FeedbackDialog(this);
	feddbackDialog->open();
}

void MainWindow::onAboutUpdateExists()
{
	UpdateLoaderDialog* updatesLoaderDialog = new UpdateLoaderDialog(this);
	updatesLoaderDialog->show();
}

void MainWindow::onAboutUpdateIsNotExists()
{
	static bool firstCall = true;

	if (firstCall)
	{
		firstCall = false;
		return;
	}

	ASSERT(ServiceLocator::instance()->isRegistered<INotificationService>());
	INotificationService* notificationService = ServiceLocator::instance()->service<INotificationService>();

	const SoftwareBranding* branding = theApp->softwareBrandingOptions();

	notificationService->info(
		tr("Information about updates"),
		tr("You have installed the latest version of the") % " " % branding->productName()
	);
}

void MainWindow::openHelpPage()
{
	QDesktopServices::openUrl(s_helpPage);
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
