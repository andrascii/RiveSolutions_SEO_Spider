#include "application.h"
#include "style_loader.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "service_locator.h"
#include "crawler.h"
#include "sequenced_data_collection.h"
#include "constants.h"
#include "preferences.h"
#include "debug_info_web_page_widget.h"
#include "settings_page_impl.h"
#include "widget_under_mouse_info.h"
#include "deferred_call.h"
#include "internet_connection_notification_manager.h"
#include "action_registry.h"
#include "action_keys.h"
#include "header_controls_container.h"
#include "main_window.h"
#include "software_branding.h"
#include "storage_adapter_factory.h"
#include "summary_data_accessor_factory.h"
#include "crawler_options.h"
#include "command_line_handler.h"
#include "command_line_keys.h"
#include "update_checker.h"
#include "update_loader_dialog.h"

namespace SeoSpider
{

namespace
{

const QByteArray s_riveSolutionsUserAgent = "RiveSolutionsBot/1.0 Alpha (+http://www.rivesolutions.com/)";

}

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_commandLineHandler(new CommandLineHandler(argc, argv))
	, m_preferences(new Preferences(this, this))
	, m_crawler(new CrawlerEngine::Crawler(Common::g_optimalParserThreadsCount, this))
	, m_sequencedDataCollection(nullptr)
	, m_softwareBrandingOptions(new SoftwareBranding)
	, m_storageAdatpterFactory(new StorageAdapterFactory)
	, m_summaryDataAccessorFactory(new SummaryDataAccessorFactory)
	, m_settings(nullptr)
	, m_translator(new QTranslator(this))
	, m_internetNotificationManager(new InternetConnectionNotificationManager(this))
	, m_headerControlsContainer(new HeaderControlsContainer())
	, m_updateChecker(new UpdateChecker(this))
{
	VERIFY(connect(m_updateChecker->qobject(), SIGNAL(updateExists(const QString&)), SLOT(onAboutUpdateExists(const QString&))));
	VERIFY(connect(m_updateChecker->qobject(), SIGNAL(updateAlreadyDownloaded(const QString&)), SLOT(onAboutUpdateAlreadyDownloaded(const QString&))));

	SplashScreen::show();

	initialize();
	initializeStyleSheet();
	showMainWindow();

	if (!m_commandLineHandler->getCommandArguments(s_openSerializedFileKey).isEmpty())
	{
		mainWindow()->openFileThroughCmd(m_commandLineHandler->getCommandArguments(s_openSerializedFileKey));
	}

	m_updateChecker->check();
}

CrawlerEngine::Crawler* Application::crawler() const noexcept
{
	return m_crawler;
}

MainWindow* Application::mainWindow() const noexcept
{
	return m_mainWindow.get();
}

CrawlerEngine::SequencedDataCollection* Application::sequencedDataCollection() const noexcept
{
	return m_sequencedDataCollection;
}

StorageAdapterFactory* Application::storageAdapterFactory() const noexcept
{
	return m_storageAdatpterFactory.get();
}

SummaryDataAccessorFactory* Application::summaryDataAccessorFactory() const noexcept
{
	return m_summaryDataAccessorFactory.get();
}

InternetConnectionNotificationManager* Application::internetConnectionNotificationManager() const noexcept
{
	return m_internetNotificationManager;
}

Preferences* Application::preferences() const noexcept
{
	return m_preferences;
}

QVariant Application::loadFromSettings(const QByteArray& key, const QVariant& defaultValue) const noexcept
{
	QVariant result = settings()->value(QLatin1String(key), defaultValue);
	DEBUGLOG << key << result.toString();
	return result;
}

void Application::saveToSettings(const QByteArray& key, const QVariant& value) noexcept
{
	settings()->setValue(QLatin1String(key), value);
}

void Application::removeKeyFromSettings(const QByteArray& key)
{
	settings()->remove(key);
}

QStringList Application::allKeys() const
{
	return settings()->allKeys();
}

HeaderControlsContainer* Application::headerControlsContainer() const
{
	return m_headerControlsContainer.get();
}

const SoftwareBranding* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

void Application::startCrawler()
{
	QAction* action = qobject_cast<QAction*>(sender());
	ASSERT(action && "This method must be called using QAction");

	const QVariant data = action->data();
	ASSERT(data.isValid() && "No data passed");

	const Url url = data.toUrl();

	if (!preferences()->crawlOutsideOfStartFolder() && 
		!url.path().isEmpty() && !url.path().endsWith(QChar('/')))
	{
		mainWindow()->showMessageBoxDialog("Invalid URL!",
			"The option \"Crawl outside of start folder\" is disabled.\n"
			"In this case the URL should be ended with '/'.",
			MessageBoxDialog::CriticalErrorIcon,
			QDialogButtonBox::Ok);

		return;
	}

	if (!internetAvailable())
	{
		mainWindow()->showMessageBoxDialog("Internet connection problem!",
			"It seems that you have some problems with internet connection.\n"
			"Please, check the connection and try again.",
			MessageBoxDialog::CriticalErrorIcon,
			QDialogButtonBox::Ok);

		return;
	}

	CrawlerOptions options;
	options.startCrawlingPage = url;

	INFOLOG << "Start crawling:" << options.startCrawlingPage.toDisplayString();

	// limit settings
	options.limitMaxUrlLength = preferences()->limitMaxUrlLength();
	options.limitSearchTotal = preferences()->limitSearchTotal();
	options.limitTimeout = preferences()->limitTimeout();
	options.maxRedirectsToFollow = preferences()->maxRedirectCount();
	options.maxLinksCountOnPage = preferences()->maxLinksCountOnPage();

	// preferences settings
	options.maxDescriptionLength = preferences()->maxDescriptionLength();
	options.minDescriptionLength = preferences()->minDescriptionLength();
	options.minTitleLength = preferences()->minTitleLength();
	options.maxTitleLength = preferences()->maxTitleLength();
	options.maxImageSizeKb = preferences()->maxImageSize();
	options.maxPageSizeKb = preferences()->maxPageSize();
	options.maxImageAltTextChars = preferences()->maxImageAltTextChars();
	options.maxH1LengthChars = preferences()->maxH1LengthChars();
	options.maxH2LengthChars = preferences()->maxH2LengthChars();

	// proxy settings
	options.useProxy = preferences()->useProxy();
	options.proxyHostName = preferences()->proxyAddress();
	options.proxyPort = preferences()->proxyPort();
	options.proxyUser = preferences()->proxyUsername();
	options.proxyPassword = preferences()->proxyPassword();

	// crawler settings
	options.checkExternalLinks = preferences()->checkExternalUrls();
	options.followInternalNofollow = preferences()->followInternalNoFollow();
	options.followExternalNofollow = preferences()->followExternalNoFollow();
	options.checkCanonicals = preferences()->checkCanonicals();
	options.checkSubdomains = preferences()->checkSubdomains();
	options.followRobotsTxtRules = preferences()->followRobotsTxt();
	options.crawlOutsideOfStartFolder = preferences()->crawlOutsideOfStartFolder();

	// robots.txt rules
	options.userAgentToFollow = static_cast<UserAgentType>(preferences()->robotSignature());

	options.parserTypeFlags.setFlag(CrawlerEngine::JavaScriptResourcesParserType, preferences()->checkJavaScript());
	options.parserTypeFlags.setFlag(CrawlerEngine::CssResourcesParserType, preferences()->checkCSS());
	options.parserTypeFlags.setFlag(CrawlerEngine::ImagesResourcesParserType, preferences()->checkImages());
	options.parserTypeFlags.setFlag(CrawlerEngine::VideoResourcesParserType, preferences()->checkSWF());
	options.parserTypeFlags.setFlag(CrawlerEngine::FlashResourcesParserType, preferences()->checkSWF());

	// User agent settings
	if (preferences()->useCustomUserAgent() && preferences()->useDesktopUserAgent())
	{
		options.userAgent = preferences()->desktopUserAgent().toLatin1();
	}
	else if (preferences()->useCustomUserAgent() && preferences()->useMobileUserAgent())
	{
		options.userAgent = preferences()->mobileUserAgent().toLatin1();
	}
	else
	{
		options.userAgent = s_riveSolutionsUserAgent;
	}

	// pause range settings
	if (preferences()->usePauseTimer())
	{
		options.pauseRangeFrom = preferences()->fromPauseTimer();
		options.pauseRangeTo = preferences()->toPauseTimer();
	}

	crawler()->startCrawling(options);
}

void Application::stopCrawler()
{
	crawler()->stopCrawling();
}

void Application::clearCrawledData()
{
	crawler()->clearData();
}

void Application::showMainWindow()
{
	SplashScreen::finish();

	m_mainWindow->showMaximized();

	emit mainWindowShown();
}

void Application::onAboutCrawlerOptionsChanged(CrawlerEngine::CrawlerOptions options)
{
	// limit settings
	preferences()->setLimitMaxUrlLength(options.limitMaxUrlLength);
	preferences()->setLimitSearchTotal(options.limitSearchTotal);
	preferences()->setLimitTimeout(options.limitTimeout);
	preferences()->setMaxRedirectCount(options.maxRedirectsToFollow);
	preferences()->setMaxLinksCountOnPage(options.maxLinksCountOnPage);

	// preferences settings
	preferences()->setMaxDescriptionLength(options.maxDescriptionLength);
	preferences()->setMinDescriptionLength(options.minDescriptionLength);
	preferences()->setMinTitleLength(options.minTitleLength);
	preferences()->setMaxTitleLength(options.maxTitleLength);
	preferences()->setMaxImageSize(options.maxImageSizeKb);
	preferences()->setMaxPageSize(options.maxPageSizeKb);
	preferences()->setMaxImageAltTextChars(options.maxImageAltTextChars);
	preferences()->setMaxH1LengthChars(options.maxH1LengthChars);
	preferences()->setMaxH2LengthChars(options.maxH2LengthChars);

	// proxy settings
	preferences()->setUseProxy(options.useProxy);
	preferences()->setProxyAddress(options.proxyHostName);
	preferences()->setProxyPort(options.proxyPort);
	preferences()->setProxyUsername(options.proxyUser);
	preferences()->setProxyPassword(options.proxyPassword);

	// crawler settings
	preferences()->setCheckExternalUrls(options.checkExternalLinks);
	preferences()->setFollowInternalNoFollow(options.followInternalNofollow);
	preferences()->setFollowExternalNoFollow(options.followExternalNofollow);
	preferences()->setCheckCanonicals(options.checkCanonicals);
	preferences()->setCheckSubdomains(options.checkSubdomains);
	preferences()->setFollowRobotsTxt(options.followRobotsTxtRules);
	preferences()->setCrawlOutsideOfStartFolder(options.crawlOutsideOfStartFolder);

	// robots.txt rules
	preferences()->setRobotSignature(static_cast<int>(options.userAgentToFollow));

	preferences()->setCheckJavaScript(options.parserTypeFlags.testFlag(CrawlerEngine::JavaScriptResourcesParserType));
	preferences()->setCheckCSS(options.parserTypeFlags.testFlag(CrawlerEngine::CssResourcesParserType));
	preferences()->setCheckImages(options.parserTypeFlags.testFlag(CrawlerEngine::ImagesResourcesParserType));
	preferences()->setCheckSWF(options.parserTypeFlags.testFlag(CrawlerEngine::VideoResourcesParserType));
	preferences()->setCheckSWF(options.parserTypeFlags.testFlag(CrawlerEngine::FlashResourcesParserType));

	// User agent settings
	if (options.userAgent == s_riveSolutionsUserAgent)
	{
		preferences()->setUseCustomUserAgent(false);
	}
	else
	{
		preferences()->setUseCustomUserAgent(true);

		if (preferences()->useMobileUserAgent())
		{
			preferences()->setMobileUserAgent(QString::fromLatin1(options.userAgent));
		}
		else
		{
			preferences()->setUseDesktopUserAgent(true);
			preferences()->setDesktopUserAgent(QString::fromLatin1(options.userAgent));
		}
	}

	// pause range settings
	if (options.pauseRangeFrom && options.pauseRangeTo)
	{
		preferences()->setUsePauseTimer(true);
		preferences()->setFromPauseTimer(options.pauseRangeFrom);
		preferences()->setToPauseTimer(options.pauseRangeTo);
	}
}

void Application::onAboutUpdateExists(const QString& downloadLink)
{
	UpdateLoaderDialog* updatesLoaderDialog = new UpdateLoaderDialog(downloadLink, mainWindow());

	VERIFY(connect(updatesLoaderDialog, &UpdateLoaderDialog::updateDownloaded, this, &Application::onAboutUpdateDownloadingFinished));

	updatesLoaderDialog->show();
}

void Application::onAboutUseCustomUserAgentChanged()
{
	if (!preferences()->useCustomUserAgent())
	{
		return;
	}

	if (preferences()->useDesktopUserAgent())
	{
		m_crawler->setUserAgent(preferences()->desktopUserAgent().toLatin1());
	}
	else if (preferences()->useMobileUserAgent())
	{
		m_crawler->setUserAgent(preferences()->mobileUserAgent().toLatin1());
	}
}

void Application::onAboutUpdateDownloadingFinished(const QString& filepath)
{
	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog;
	messageBoxDialog->setWindowTitle(tr("Updates successful downloaded"));
	messageBoxDialog->setMessage(tr("Updates successful downloaded. Do you want to install the updates now?"));
	messageBoxDialog->setIcon(MessageBoxDialog::InformationIcon);
	messageBoxDialog->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	messageBoxDialog->exec();

	if (messageBoxDialog->result() != QDialog::Accepted)
	{
		return;
	}

	if (QProcess::startDetached(filepath))
	{
		quit();
	}
}

void Application::onAboutUpdateAlreadyDownloaded(const QString& filepath)
{
	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog;
	messageBoxDialog->setWindowTitle(tr("New updates already downloaded"));
	messageBoxDialog->setMessage(tr("New updates already downloaded. Do you want to install the updates now?"));
	messageBoxDialog->setIcon(MessageBoxDialog::InformationIcon);
	messageBoxDialog->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	messageBoxDialog->exec();

	if (messageBoxDialog->result() != QDialog::Accepted)
	{
		return;
	}

	if (QProcess::startDetached(filepath))
	{
		quit();
	}
}

void Application::registerServices()
{
	ServiceLocator::instance()->addService<ISettingsPageRegistry>(new SettingsPageRegistry);
}

void Application::initQSettings()
{
	m_settings = new QSettings(
		softwareBrandingOptions()->organizationName(), 
		softwareBrandingOptions()->productName(),
		qobject_cast<QObject*>(this));
}

QSettings* Application::settings() const
{
	ASSERT(m_settings);
	return m_settings;
}

void Application::initialize()
{
	DeferredCallProcessor::init();

	m_crawler->initialize();

	// must be Qt::QueuedConnection
	VERIFY(connect(preferences(), &Preferences::useCustomUserAgentChanged, 
		this, &Application::onAboutUseCustomUserAgentChanged, Qt::QueuedConnection));

	m_sequencedDataCollection = m_crawler->sequencedDataCollection();

	SplashScreen::showMessage("Initializing...");

#ifdef PRODUCTION
	std::this_thread::sleep_for(3s);
#endif

	registerServices();
	initQSettings();
	preferences()->load();

	m_translator->load(":/translations/translate_" + preferences()->applicationLanguage());
	installTranslator(m_translator);

	SplashScreen::showMessage("Loading main window...");

	m_mainWindow.reset(new MainWindow);

#if !defined(PRODUCTION)

	StyleLoader::attach(QStringLiteral("styles.css"), QStringLiteral("F5"));
	WidgetUnderMouseInfo::attach(QStringLiteral("F6"));

#endif

	VERIFY(connect(m_crawler, &Crawler::crawlerOptionsChanged, this, &Application::onAboutCrawlerOptionsChanged));

	mainWindow()->init();
}

void Application::initializeStyleSheet() noexcept
{
	SplashScreen::showMessage("Initializing stylesheets...");
	QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
	QFile styles(":/stylesheets/styles.css");

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(styles.readAll());
	}
}

bool Application::internetAvailable() const noexcept
{
	return m_internetNotificationManager->internetAvailable();
}

Application::~Application()
{
	ServiceLocator::instance()->destroyService<ISettingsPageRegistry>();
	DeferredCallProcessor::term();
}

}
