#include "stdafx.h"
#include "application.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "service_locator.h"
#include "crawler.h"
#include "sequenced_data_collection.h"
#include "preferences.h"
#include "settings_page_impl.h"
#include "deferred_call.h"
#include "internet_connection_notification_manager.h"
#include "main_window.h"
#include "storage_adapter_factory.h"
#include "summary_data_accessor_factory.h"
#include "crawler_options.h"
#include "command_line_handler.h"
#include "command_line_keys.h"
#include "smtp_sender.h"
#include "wait_operation_frame.h"
#include "version.h"
#include "license_state_notificator.h"
#include "dialog.h"

#ifndef PRODUCTION
#include "style_loader.h"
#include "widget_under_mouse_info.h"
#endif

namespace
{

const QString s_riveSolutionsUserAgent = QString("RiveSolutionsBot/%1 Alpha (+http://www.rivesolutions.com/)").arg(VERSION_STR);

}

namespace SeoSpider
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_commandLineHandler(new CommandLineHandler(argc, argv))
	, m_preferences(new Preferences(this, this))
	, m_crawler(new CrawlerEngine::Crawler(this))
	, m_sequencedDataCollection(nullptr)
	, m_softwareBrandingOptions(new SoftwareBranding)
	, m_storageAdatpterFactory(new StorageAdapterFactory)
	, m_summaryDataAccessorFactory(new SummaryDataAccessorFactory)
	, m_settings(nullptr)
	, m_translator(new QTranslator(this))
	, m_internetNotificationManager(new InternetConnectionNotificationManager(this))
	, m_curlSuccessfullyInitialized(false)
{
	m_curlSuccessfullyInitialized = !m_commandLineHandler->hasCommand(s_useOldDownloader) ?
		curl_global_init(CURL_GLOBAL_ALL) == 0 : false;

	m_crawler->setDownloaderType(m_curlSuccessfullyInitialized ?
		CrawlerEngine::Crawler::DownloaderTypeLibCurlMultiSocket :
		CrawlerEngine::Crawler::DownloaderTypeQNetworkAccessManager);

	if (m_commandLineHandler->hasCommand(s_workerCount))
	{
		const unsigned workerCount = m_commandLineHandler->commandValue(s_workerCount).toUInt();
		m_crawler->setWorkerCount(workerCount);
	}

	Common::SmtpSender::init();

	initializeStyleSheet();

#ifdef PRODUCTION
	SplashScreen::show();
#endif

	attachPreferencesToCrawlerOptions();
	initialize();
	showMainWindow();

#ifdef SUPPORT_SERIALIZATION
	if (!m_commandLineHandler->commandValue(s_openSerializedFileKey).isEmpty())
	{
		openFileThroughCmd(m_commandLineHandler->commandValue(s_openSerializedFileKey));
	}
#endif

	new LicenseStateNotificator(this);
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

const CommandLineHandler* Application::commandLineHandler() const
{
	return m_commandLineHandler.get();
}

const SoftwareBranding* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

const QString Application::getOrGenerateUserID()
{
	//
	//TODO: find out hou to change this to loadFromSettings("userID", QUuid::createUuid());
	//

	if(!loadFromSettings("userID").isValid())
	{
		saveToSettings("userID", QUuid::createUuid());
	}

	return loadFromSettings("userID").toString();
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
		Dialog::showMessageBoxDialog(tr("Error"),
			tr("Please make sure that you entered a URL."),
			QDialogButtonBox::Ok
		);

		return;
	}

	const Url url = data.toUrl();

	if (!preferences()->crawlOutsideOfStartFolder() &&
		!url.path().isEmpty() && !url.path().endsWith(QChar('/')))
	{
		Dialog::showMessageBoxDialog(tr("Invalid URL!"),
			tr("The option \"Crawl outside of start folder\" is disabled. "
			"In this case the URL should be ended with '/'."),
			QDialogButtonBox::Ok);

		return;
	}

	if (!isInternetAvailable())
	{
		Dialog::showMessageBoxDialog("Internet connection problem!",
			tr("It seems that you have some problems with internet connection. "
			"Please, check the connection and try again."),
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
	if (theApp->crawler()->state() == Crawler::StatePause ||
		theApp->crawler()->state() == Crawler::StatePending)
	{
		return;
	}

	WaitOperationFrame::showMessage(tr("Stopping crawler..."));

	crawler()->stopCrawling();
}

void Application::clearCrawledData()
{
	crawler()->clearData();
}

void Application::restartCrawler()
{
	clearCrawledData();
	startCrawler();
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
	preferences()->setCheckVideo(parserTypeFlags.testFlag(CrawlerEngine::VideoResourcesParserType));
	preferences()->setCheckSWF(parserTypeFlags.testFlag(CrawlerEngine::FlashResourcesParserType));
	preferences()->setCheckOther(parserTypeFlags.testFlag(CrawlerEngine::OtherResourcesParserType));

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
	preferences()->setUsePauseTimer(true);
	preferences()->setFromPauseTimer(crawler()->options()->pauseRangeEnabled());
	preferences()->setToPauseTimer(crawler()->options()->pauseRangeTo());

	// yandex metrica counters

	const bool isAllYmCountersDisabled =
		!crawler()->options()->searchYandexMetricaCounter1() &&
		!crawler()->options()->searchYandexMetricaCounter2() &&
		!crawler()->options()->searchYandexMetricaCounter3() &&
		!crawler()->options()->searchYandexMetricaCounter4() &&
		!crawler()->options()->searchYandexMetricaCounter5();

	preferences()->setSearchYandexMetricaCounters(crawler()->options()->searchYandexMetricaCounters());
	preferences()->setSearchYandexMetricaCounter1(crawler()->options()->searchYandexMetricaCounter1());
	preferences()->setYandexMetricaCounter1Id(crawler()->options()->yandexMetricaCounter1Id());
	preferences()->setSearchYandexMetricaCounter2(crawler()->options()->searchYandexMetricaCounter2());
	preferences()->setYandexMetricaCounter2Id(crawler()->options()->yandexMetricaCounter2Id());
	preferences()->setSearchYandexMetricaCounter3(crawler()->options()->searchYandexMetricaCounter3());
	preferences()->setYandexMetricaCounter3Id(crawler()->options()->yandexMetricaCounter3Id());
	preferences()->setSearchYandexMetricaCounter4(crawler()->options()->searchYandexMetricaCounter4());
	preferences()->setYandexMetricaCounter4Id(crawler()->options()->yandexMetricaCounter4Id());
	preferences()->setSearchYandexMetricaCounter5(crawler()->options()->searchYandexMetricaCounter5());
	preferences()->setYandexMetricaCounter5Id(crawler()->options()->yandexMetricaCounter5Id());

	if (isAllYmCountersDisabled)
	{
		preferences()->setSearchYandexMetricaCounter1(true);
	}

	// google analytics counters

	const bool isAllGaCountersDisabled =
		!crawler()->options()->searchGoogleAnalyticsCounter1() &&
		!crawler()->options()->searchGoogleAnalyticsCounter2() &&
		!crawler()->options()->searchGoogleAnalyticsCounter3() &&
		!crawler()->options()->searchGoogleAnalyticsCounter4() &&
		!crawler()->options()->searchGoogleAnalyticsCounter5();

	preferences()->setSearchGoogleAnalyticsCounters(crawler()->options()->searchGoogleAnalyticsCounters());
	preferences()->setSearchGoogleAnalyticsCounter1(crawler()->options()->searchGoogleAnalyticsCounter1());
	preferences()->setGoogleAnalyticsCounter1Id(crawler()->options()->googleAnalyticsCounter1Id());
	preferences()->setSearchGoogleAnalyticsCounter2(crawler()->options()->searchGoogleAnalyticsCounter2());
	preferences()->setGoogleAnalyticsCounter2Id(crawler()->options()->googleAnalyticsCounter2Id());
	preferences()->setSearchGoogleAnalyticsCounter3(crawler()->options()->searchGoogleAnalyticsCounter3());
	preferences()->setGoogleAnalyticsCounter3Id(crawler()->options()->googleAnalyticsCounter3Id());
	preferences()->setSearchGoogleAnalyticsCounter4(crawler()->options()->searchGoogleAnalyticsCounter4());
	preferences()->setGoogleAnalyticsCounter4Id(crawler()->options()->googleAnalyticsCounter4Id());
	preferences()->setSearchGoogleAnalyticsCounter5(crawler()->options()->searchGoogleAnalyticsCounter5());
	preferences()->setGoogleAnalyticsCounter5Id(crawler()->options()->googleAnalyticsCounter5Id());

	if (isAllGaCountersDisabled)
	{
		preferences()->setSearchGoogleAnalyticsCounter1(true);
	}
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

	VERIFY(connect(m_crawler, &Crawler::crawlerStarted, this, &Application::closeWaitOperationFrame));
	VERIFY(connect(m_crawler, &Crawler::crawlerStopped, this, &Application::closeWaitOperationFrame));
	VERIFY(connect(m_crawler, &Crawler::crawlerFinished, this, &Application::closeWaitOperationFrame));
	VERIFY(connect(m_crawler, &Crawler::crawlerFailed, this, &Application::closeWaitOperationFrame));
	VERIFY(connect(m_crawler, &Crawler::refreshPageDone, this, &Application::closeWaitOperationFrame));

	/// must be Qt::QueuedConnection
	VERIFY(connect(preferences(), &Preferences::useCustomUserAgentChanged,
		this, &Application::onAboutUseCustomUserAgentChanged, Qt::QueuedConnection));

	m_sequencedDataCollection = m_crawler->sequencedDataCollection();

	SplashScreen::showMessage("Initializing...");

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

	applicationInitialized();
}

void Application::applicationInitialized()
{
	Common::SeoSpiderServiceApiLoader::serviceApi()->applicationInitialized(
		getOrGenerateUserID().toLatin1(),
		QLocale::countryToString(QLocale::system().country()).toLatin1(),
		QLocale::languageToString(QLocale::system().language()).toLatin1(),
		QSysInfo::prettyProductName().toLatin1(),
		QString("%1").arg(QSysInfo::WordSize).toLatin1(),
		VERSION_STR);
}

void Application::attachPreferencesToCrawlerOptions()
{
	ASSERT(crawler());

	VERIFY(connect(preferences(), SIGNAL(limitMaxUrlLengthChanged(int)), crawler()->options()->qobject(), SLOT(setLimitMaxUrlLength(int))));
	VERIFY(connect(preferences(), SIGNAL(limitSearchTotalChanged(int)), crawler()->options()->qobject(), SLOT(setLimitSearchTotal(int))));
	VERIFY(connect(preferences(), SIGNAL(limitTimeoutChanged(int)), crawler()->options()->qobject(), SLOT(setLimitTimeout(int))));
	VERIFY(connect(preferences(), SIGNAL(maxRedirectCountChanged(int)), crawler()->options()->qobject(), SLOT(setMaxRedirectsToFollow(int))));
	VERIFY(connect(preferences(), SIGNAL(maxParallelConnectionsChanged(int)), crawler()->options()->qobject(), SLOT(setMaxParallelConnections(int))));
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
	VERIFY(connect(preferences(), SIGNAL(crawlMetaHrefLangLinksChanged(bool)), crawler()->options()->qobject(), SLOT(setCrawlMetaHrefLangLinks(bool))));
	VERIFY(connect(preferences(), SIGNAL(searchYandexMetricaCountersChanged(bool)), crawler()->options()->qobject(), SLOT(setSearchYandexMetricaCounters(bool))));
	VERIFY(connect(preferences(), SIGNAL(searchYandexMetricaCounter1Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchYandexMetricaCounter1(bool))));
	VERIFY(connect(preferences(), SIGNAL(yandexMetricaCounter1IdChanged(int)), crawler()->options()->qobject(), SLOT(setYandexMetricaCounter1Id(int))));
	VERIFY(connect(preferences(), SIGNAL(searchYandexMetricaCounter2Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchYandexMetricaCounter2(bool))));
	VERIFY(connect(preferences(), SIGNAL(yandexMetricaCounter2IdChanged(int)), crawler()->options()->qobject(), SLOT(setYandexMetricaCounter2Id(int))));
	VERIFY(connect(preferences(), SIGNAL(searchYandexMetricaCounter3Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchYandexMetricaCounter3(bool))));
	VERIFY(connect(preferences(), SIGNAL(yandexMetricaCounter3IdChanged(int)), crawler()->options()->qobject(), SLOT(setYandexMetricaCounter3Id(int))));
	VERIFY(connect(preferences(), SIGNAL(searchYandexMetricaCounter4Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchYandexMetricaCounter4(bool))));
	VERIFY(connect(preferences(), SIGNAL(yandexMetricaCounter4IdChanged(int)), crawler()->options()->qobject(), SLOT(setYandexMetricaCounter4Id(int))));
	VERIFY(connect(preferences(), SIGNAL(searchYandexMetricaCounter5Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchYandexMetricaCounter5(bool))));
	VERIFY(connect(preferences(), SIGNAL(yandexMetricaCounter5IdChanged(int)), crawler()->options()->qobject(), SLOT(setYandexMetricaCounter5Id(int))));
	VERIFY(connect(preferences(), SIGNAL(searchGoogleAnalyticsCountersChanged(bool)), crawler()->options()->qobject(), SLOT(setSearchGoogleAnalyticsCounters(bool))));
	VERIFY(connect(preferences(), SIGNAL(searchGoogleAnalyticsCounter1Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchGoogleAnalyticsCounter1(bool))));
	VERIFY(connect(preferences(), SIGNAL(googleAnalyticsCounter1IdChanged(const QString&)), crawler()->options()->qobject(), SLOT(setGoogleAnalyticsCounter1Id(const QString&))));
	VERIFY(connect(preferences(), SIGNAL(searchGoogleAnalyticsCounter2Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchGoogleAnalyticsCounter2(bool))));
	VERIFY(connect(preferences(), SIGNAL(googleAnalyticsCounter2IdChanged(const QString&)), crawler()->options()->qobject(), SLOT(setGoogleAnalyticsCounter2Id(const QString&))));
	VERIFY(connect(preferences(), SIGNAL(searchGoogleAnalyticsCounter3Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchGoogleAnalyticsCounter3(bool))));
	VERIFY(connect(preferences(), SIGNAL(googleAnalyticsCounter3IdChanged(const QString&)), crawler()->options()->qobject(), SLOT(setGoogleAnalyticsCounter3Id(const QString&))));
	VERIFY(connect(preferences(), SIGNAL(searchGoogleAnalyticsCounter4Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchGoogleAnalyticsCounter4(bool))));
	VERIFY(connect(preferences(), SIGNAL(googleAnalyticsCounter4IdChanged(const QString&)), crawler()->options()->qobject(), SLOT(setGoogleAnalyticsCounter4Id(const QString&))));
	VERIFY(connect(preferences(), SIGNAL(searchGoogleAnalyticsCounter5Changed(bool)), crawler()->options()->qobject(), SLOT(setSearchGoogleAnalyticsCounter5(bool))));
	VERIFY(connect(preferences(), SIGNAL(googleAnalyticsCounter5IdChanged(const QString&)), crawler()->options()->qobject(), SLOT(setGoogleAnalyticsCounter5Id(const QString&))));

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
		parserTypeFlags.setFlag(CrawlerEngine::VideoResourcesParserType, preferences()->checkVideo());
		parserTypeFlags.setFlag(CrawlerEngine::FlashResourcesParserType, preferences()->checkSWF());
		parserTypeFlags.setFlag(CrawlerEngine::OtherResourcesParserType, preferences()->checkOther());

		crawler()->options()->setParserTypeFlags(parserTypeFlags);
	};

	VERIFY(connect(preferences(), &Preferences::checkJavaScriptChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkCSSChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkImagesChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkSWFChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkVideoChanged, parserTypeFlagsMapper));
	VERIFY(connect(preferences(), &Preferences::checkOtherChanged, parserTypeFlagsMapper));

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
			crawler()->options()->setUserAgent(s_riveSolutionsUserAgent.toLatin1());
		}
	};

	VERIFY(connect(preferences(), &Preferences::useCustomUserAgentChanged, userAgentMapper));
	VERIFY(connect(preferences(), &Preferences::useDesktopUserAgentChanged, userAgentMapper));

	const auto pauseSetFromValue = [this](int value)
	{
		crawler()->options()->setPauseRangeFrom(value);
	};

	const auto pauseSetToValue = [this](int value)
	{
		crawler()->options()->setPauseRangeTo(value);
	};

	const auto pauseSetEnabledValue = [this](bool enabled)
	{
		crawler()->options()->setPauseRangeEnabled(enabled);
	};

	VERIFY(connect(preferences(), &Preferences::usePauseTimerChanged, pauseSetEnabledValue));
	VERIFY(connect(preferences(), &Preferences::fromPauseTimerChanged, pauseSetFromValue));
	VERIFY(connect(preferences(), &Preferences::toPauseTimerChanged, pauseSetToValue));
}

void Application::openFileThroughCmd(const QString& path)
{
	if (!path.endsWith(c_projectFileExtension))
	{
		ERRLOG << path;

		Dialog::showMessageBoxDialog(
			tr("Error"),
			tr("Cannot open! Unknown document type."),
			QDialogButtonBox::Ok
		);

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

	if (m_curlSuccessfullyInitialized)
	{
		curl_global_cleanup();
	}
}

}
