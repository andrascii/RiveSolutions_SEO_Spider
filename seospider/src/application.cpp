#include "application.h"
#include "style_loader.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "service_locator.h"
#include "crawler.h"
#include "sequenced_data_collection.h"
#include "constants.h"
#include "common_constants.h"
#include "preferences.h"
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
#include "license_service.h"
#include "smtp_sender.h"
#include "wait_operation_frame.h"

namespace
{

const QByteArray s_riveSolutionsUserAgent = "RiveSolutionsBot/1.0 Alpha (+http://www.rivesolutions.com/)";

}

namespace SeoSpider
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_commandLineHandler(new CommandLineHandler(argc, argv))
	, m_preferences(new Preferences(this, this))
	, m_crawler(new CrawlerEngine::Crawler(c_optimalParserThreadsCount, this))
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
	Common::SmtpSender::init();

	qRegisterMetaType<Version>("Version");

	VERIFY(connect(m_updateChecker->qobject(), SIGNAL(updateExists()), SLOT(onAboutUpdateExists())));

	initializeStyleSheet();
	SplashScreen::show();

	std::this_thread::sleep_for(10s);

	attachPreferencesToCrawlerOptions();
	initialize();
	showMainWindow();

	if (!m_commandLineHandler->getCommandArguments(s_openSerializedFileKey).isEmpty())
	{
		openFileThroughCmd(m_commandLineHandler->getCommandArguments(s_openSerializedFileKey));
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

HeaderControlsContainer* Application::headerControlsContainer() const noexcept
{
	return m_headerControlsContainer.get();
}

const SoftwareBranding* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

void Application::startCrawler()
{
	if (crawler()->state() == Crawler::StateWorking ||
		crawler()->state() == Crawler::StatePreChecking)
	{
		return;
	}

	QAction* action = qobject_cast<QAction*>(sender());
	ASSERT(action && "This method must be called using QAction");

	const QVariant data = action->data();

	if (!data.isValid())
	{
		mainWindow()->showMessageBoxDialog("Error", "Please make sure that you entered a URL.",
			MessageBoxDialog::CriticalErrorIcon,
			QDialogButtonBox::Ok);

		return;
	}

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

	if (!isInternetAvailable())
	{
		mainWindow()->showMessageBoxDialog("Internet connection problem!",
			"It seems that you have some problems with internet connection.\n"
			"Please, check the connection and try again.",
			MessageBoxDialog::CriticalErrorIcon,
			QDialogButtonBox::Ok);

		return;
	}

	if (crawler()->state() != Crawler::StatePause)
	{
		crawler()->options()->setStartCrawlingPage(url);
	}

	INFOLOG << "Start crawling:" << crawler()->options()->startCrawlingPage().toDisplayString();

	crawler()->startCrawling();

	WaitOperationFrame::showMessage(tr("Starting crawler..."));
}

void Application::stopCrawler()
{
	WaitOperationFrame::showMessage(tr("Stopping crawler..."));

	crawler()->stopCrawling();
}

void Application::clearCrawledData()
{
	crawler()->clearData();
}

void Application::showMainWindow()
{
	SplashScreen::finish();

	m_mainWindow->show();

	emit mainWindowShown();
}

void Application::onAboutCrawlerOptionsChanged()
{
	// limit settings
	preferences()->setLimitMaxUrlLength(crawler()->options()->limitMaxUrlLength());
	preferences()->setLimitSearchTotal(crawler()->options()->limitSearchTotal());
	preferences()->setLimitTimeout(crawler()->options()->limitTimeout());
	preferences()->setMaxRedirectCount(crawler()->options()->maxRedirectsToFollow());
	preferences()->setMaxLinksCountOnPage(crawler()->options()->maxLinksCountOnPage());

	// preferences settings
	preferences()->setMaxDescriptionLength(crawler()->options()->maxDescriptionLength());
	preferences()->setMinDescriptionLength(crawler()->options()->minDescriptionLength());
	preferences()->setMinTitleLength(crawler()->options()->minTitleLength());
	preferences()->setMaxTitleLength(crawler()->options()->maxTitleLength());
	preferences()->setMaxImageSize(crawler()->options()->maxImageSizeKb());
	preferences()->setMaxPageSize(crawler()->options()->maxPageSizeKb());
	preferences()->setMaxImageAltTextChars(crawler()->options()->maxImageAltTextChars());
	preferences()->setMaxH1LengthChars(crawler()->options()->maxH1LengthChars());
	preferences()->setMaxH2LengthChars(crawler()->options()->maxH2LengthChars());

	// proxy settings
	preferences()->setUseProxy(crawler()->options()->useProxy());
	preferences()->setProxyAddress(crawler()->options()->proxyHostName());
	preferences()->setProxyPort(crawler()->options()->proxyPort());
	preferences()->setProxyUsername(crawler()->options()->proxyUser());
	preferences()->setProxyPassword(crawler()->options()->proxyPassword());

	// crawler settings
	preferences()->setCheckExternalUrls(crawler()->options()->checkExternalLinks());
	preferences()->setFollowInternalNoFollow(crawler()->options()->followInternalNofollow());
	preferences()->setFollowExternalNoFollow(crawler()->options()->followExternalNofollow());
	preferences()->setCheckCanonicals(crawler()->options()->checkCanonicals());
	preferences()->setCheckSubdomains(crawler()->options()->checkSubdomains());
	preferences()->setFollowRobotsTxt(crawler()->options()->followRobotsTxtRules());
	preferences()->setCrawlOutsideOfStartFolder(crawler()->options()->crawlOutsideOfStartFolder());

	// robots.txt rules
	preferences()->setRobotSignature(static_cast<int>(crawler()->options()->userAgentToFollow()));

	ParserTypeFlags parserTypeFlags = crawler()->options()->parserTypeFlags();

	preferences()->setCheckJavaScript(parserTypeFlags.testFlag(CrawlerEngine::JavaScriptResourcesParserType));
	preferences()->setCheckCSS(parserTypeFlags.testFlag(CrawlerEngine::CssResourcesParserType));
	preferences()->setCheckImages(parserTypeFlags.testFlag(CrawlerEngine::ImagesResourcesParserType));
	preferences()->setCheckSWF(parserTypeFlags.testFlag(CrawlerEngine::VideoResourcesParserType));
	preferences()->setCheckSWF(parserTypeFlags.testFlag(CrawlerEngine::FlashResourcesParserType));

	// User agent settings
	if (crawler()->options()->userAgent() == s_riveSolutionsUserAgent)
	{
		preferences()->setUseCustomUserAgent(false);
	}
	else
	{
		preferences()->setUseCustomUserAgent(true);

		if (preferences()->useMobileUserAgent())
		{
			preferences()->setMobileUserAgent(QString::fromLatin1(crawler()->options()->userAgent()));
		}
		else
		{
			preferences()->setUseDesktopUserAgent(true);
			preferences()->setDesktopUserAgent(QString::fromLatin1(crawler()->options()->userAgent()));
		}
	}

	// pause range settings
	if (crawler()->options()->pauseRangeFrom() && crawler()->options()->pauseRangeTo())
	{
		preferences()->setUsePauseTimer(true);
		preferences()->setFromPauseTimer(crawler()->options()->pauseRangeFrom());
		preferences()->setToPauseTimer(crawler()->options()->pauseRangeTo());
	}
}

void Application::onAboutUpdateExists()
{
	UpdateLoaderDialog* updatesLoaderDialog = new UpdateLoaderDialog(mainWindow());

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

void Application::closeWaitOperationFrame()
{
	WaitOperationFrame::finish();
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

	registerServices();

	m_crawler->initialize();

	VERIFY(connect(m_crawler, &Crawler::crawlerStarted,
		this, &Application::closeWaitOperationFrame, Qt::QueuedConnection));

	VERIFY(connect(m_crawler, &Crawler::crawlerStopped,
		this, &Application::closeWaitOperationFrame, Qt::QueuedConnection));
	
	VERIFY(connect(m_crawler, &Crawler::crawlerFinished,
		this, &Application::closeWaitOperationFrame, Qt::QueuedConnection));

	/// must be Qt::QueuedConnection
	VERIFY(connect(preferences(), &Preferences::useCustomUserAgentChanged, 
		this, &Application::onAboutUseCustomUserAgentChanged, Qt::QueuedConnection));

	m_sequencedDataCollection = m_crawler->sequencedDataCollection();

	SplashScreen::showMessage("Initializing...");

#ifdef PRODUCTION
	std::this_thread::sleep_for(3s);
#endif

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

	VERIFY(connect(m_crawler, &Crawler::crawlerOptionsLoaded, this, &Application::onAboutCrawlerOptionsChanged));

	mainWindow()->init();
}

void Application::startInstaller(const QString& filepath)
{
	if (QProcess::startDetached(filepath))
	{
		quit();
	}
}

void Application::attachPreferencesToCrawlerOptions()
{
	ASSERT(crawler());

	VERIFY(connect(preferences(), SIGNAL(limitMaxUrlLengthChanged(int)), crawler()->options()->qobject(), SLOT(setLimitMaxUrlLength(int))));
	VERIFY(connect(preferences(), SIGNAL(limitSearchTotalChanged(int)), crawler()->options()->qobject(), SLOT(setLimitSearchTotal(int))));
	VERIFY(connect(preferences(), SIGNAL(limitTimeoutChanged(int)), crawler()->options()->qobject(), SLOT(setLimitTimeout(int))));
	VERIFY(connect(preferences(), SIGNAL(maxRedirectCountChanged(int)), crawler()->options()->qobject(), SLOT(setMaxRedirectsToFollow(int))));
	VERIFY(connect(preferences(), SIGNAL(maxLinksCountOnPageChanged(int)), crawler()->options()->qobject(), SLOT(setMaxLinksCountOnPage(int))));
	VERIFY(connect(preferences(), SIGNAL(maxDescriptionLengthChanged(int)), crawler()->options()->qobject(), SLOT(setMaxDescriptionLength(int))));
	VERIFY(connect(preferences(), SIGNAL(minDescriptionLengthChanged(int)), crawler()->options()->qobject(), SLOT(setMinDescriptionLength(int))));
	VERIFY(connect(preferences(), SIGNAL(minTitleLengthChanged(int)), crawler()->options()->qobject(), SLOT(setMinTitleLength(int))));
	VERIFY(connect(preferences(), SIGNAL(maxTitleLengthChanged(int)), crawler()->options()->qobject(), SLOT(setMaxTitleLength(int))));
	VERIFY(connect(preferences(), SIGNAL(maxImageSizeChanged(int)), crawler()->options()->qobject(), SLOT(setMaxImageSizeKb(int))));
	VERIFY(connect(preferences(), SIGNAL(maxPageSizeChanged(int)), crawler()->options()->qobject(), SLOT(setMaxPageSizeKb(int))));
	VERIFY(connect(preferences(), SIGNAL(maxImageAltTextCharsChanged(int)), crawler()->options()->qobject(), SLOT(setMaxImageAltTextChars(int))));
	VERIFY(connect(preferences(), SIGNAL(maxH1LengthCharsChanged(int)), crawler()->options()->qobject(), SLOT(setMaxH1LengthChars(int))));
	VERIFY(connect(preferences(), SIGNAL(maxH2LengthCharsChanged(int)), crawler()->options()->qobject(), SLOT(setMaxH2LengthChars(int))));
	VERIFY(connect(preferences(), SIGNAL(useProxyChanged(bool)), crawler()->options()->qobject(), SLOT(setUseProxy(bool))));
	VERIFY(connect(preferences(), SIGNAL(proxyAddressChanged(QString)), crawler()->options()->qobject(), SLOT(setProxyHostName(QString))));
	VERIFY(connect(preferences(), SIGNAL(proxyPortChanged(int)), crawler()->options()->qobject(), SLOT(setProxyPort(int))));
	VERIFY(connect(preferences(), SIGNAL(proxyUsernameChanged(QString)), crawler()->options()->qobject(), SLOT(setProxyUser(QString))));
	VERIFY(connect(preferences(), SIGNAL(proxyPasswordChanged(QString)), crawler()->options()->qobject(), SLOT(setProxyPassword(QString))));
	VERIFY(connect(preferences(), SIGNAL(checkExternalUrlsChanged(bool)), crawler()->options()->qobject(), SLOT(setCheckExternalLinks(bool))));
	VERIFY(connect(preferences(), SIGNAL(followInternalNoFollowChanged(bool)), crawler()->options()->qobject(), SLOT(setFollowInternalNofollow(bool))));
	VERIFY(connect(preferences(), SIGNAL(followExternalNoFollowChanged(bool)), crawler()->options()->qobject(), SLOT(setFollowExternalNofollow(bool))));
	VERIFY(connect(preferences(), SIGNAL(checkCanonicalsChanged(bool)), crawler()->options()->qobject(), SLOT(setCheckCanonicals(bool))));
	VERIFY(connect(preferences(), SIGNAL(checkSubdomainsChanged(bool)), crawler()->options()->qobject(), SLOT(setCheckSubdomains(bool))));
	VERIFY(connect(preferences(), SIGNAL(followRobotsTxtChanged(bool)), crawler()->options()->qobject(), SLOT(setFollowRobotsTxtRules(bool))));
	VERIFY(connect(preferences(), SIGNAL(crawlOutsideOfStartFolderChanged(bool)), crawler()->options()->qobject(), SLOT(setCrawlOutsideOfStartFolder(bool))));

	const auto mapVariantToUserAgentType = [this](const QVariant& value)
	{
		DEBUG_ASSERT(value.type() == QVariant::Int || value.type() == QVariant::String);

		if (value.type() != QVariant::Int)
		{
			return;
		}

		CrawlerEngine::UserAgentType userAgentType = static_cast<CrawlerEngine::UserAgentType>(value.toInt());

		DEBUG_ASSERT(userAgentType >= CrawlerEngine::UserAgentType::Unknown && 
			userAgentType <= CrawlerEngine::UserAgentType::AnyBot);

		crawler()->options()->setUserAgentToFollow(userAgentType);
	};

	VERIFY(connect(preferences(), &Preferences::robotSignatureChanged, mapVariantToUserAgentType));

	const auto parserTypeFlagsMapper = [this](bool)
	{
		ParserTypeFlags parserTypeFlags;
		parserTypeFlags.setFlag(CrawlerEngine::JavaScriptResourcesParserType, preferences()->checkJavaScript());
		parserTypeFlags.setFlag(CrawlerEngine::CssResourcesParserType, preferences()->checkCSS());
		parserTypeFlags.setFlag(CrawlerEngine::ImagesResourcesParserType, preferences()->checkImages());
		parserTypeFlags.setFlag(CrawlerEngine::VideoResourcesParserType, preferences()->checkSWF());
		parserTypeFlags.setFlag(CrawlerEngine::FlashResourcesParserType, preferences()->checkSWF());

		crawler()->options()->setParserTypeFlags(parserTypeFlags);
	};

	VERIFY(connect(preferences(), &Preferences::checkJavaScriptChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkCSSChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkImagesChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkSWFChanged, parserTypeFlagsMapper));

	const auto userAgentMapper = [this](bool)
	{
		if (preferences()->useCustomUserAgent() && preferences()->useDesktopUserAgent())
		{
			crawler()->options()->setUserAgent(preferences()->desktopUserAgent().toLatin1());
		}
		else if (preferences()->useCustomUserAgent() && preferences()->useMobileUserAgent())
		{
			crawler()->options()->setUserAgent(preferences()->mobileUserAgent().toLatin1());
		}
		else
		{
			crawler()->options()->setUserAgent(s_riveSolutionsUserAgent);
		}
	};

	VERIFY(connect(preferences(), &Preferences::useCustomUserAgentChanged, userAgentMapper));
	VERIFY(connect(preferences(), &Preferences::useDesktopUserAgentChanged, userAgentMapper));

	const auto usePauseMapper = [this](bool value)
	{
		const int fromPauseTimerValue = value ? preferences()->fromPauseTimer() : -1;
		const int toPauseTimerValue = value ? preferences()->toPauseTimer() : -1;

		DEBUGLOG << "from pause:" << fromPauseTimerValue;
		DEBUGLOG << "to pause:" << toPauseTimerValue;

		crawler()->options()->setPauseRangeFrom(fromPauseTimerValue);
		crawler()->options()->setPauseRangeTo(toPauseTimerValue);
	};

	VERIFY(connect(preferences(), &Preferences::usePauseTimerChanged, usePauseMapper));
}

void Application::openFileThroughCmd(const QString& path)
{
	if (!path.endsWith(c_projectFileExtension))
	{
		ERRLOG << path;

		mainWindow()->showMessageBoxDialog(tr("Error"), tr("Cannot open! Unknown document type."),
			MessageBoxDialog::CriticalErrorIcon, QDialogButtonBox::Ok);

		return;
	}

	crawler()->loadFromFile(path);
}

void Application::initializeStyleSheet()
{
	SplashScreen::showMessage("Initializing stylesheets...");
	QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
	QFile styles(":/stylesheets/styles.css");

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(styles.readAll());
	}
}

bool Application::isInternetAvailable() const noexcept
{
	return m_internetNotificationManager->internetAvailable();
}

Application::~Application()
{
	ServiceLocator::instance()->destroyService<ISettingsPageRegistry>();
	DeferredCallProcessor::term();
	Common::SmtpSender::term();
}

}
