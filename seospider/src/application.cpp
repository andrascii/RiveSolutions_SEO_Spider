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
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "deferred_call.h"
#include "internet_connection_notification_manager.h"
#include "web_screenshot.h"
#include "action_registry.h"
#include "action_keys.h"

namespace SeoSpider
{

namespace
{

const QByteArray s_riveSolutionsUserAgent = "RiveSolutionsBot/1.0 Alpha (+http://www.rivesolutions.com/)";

}

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_preferences(new Preferences(this, this))
	, m_crawler(new CrawlerEngine::Crawler(Common::g_optimalParserThreadsCount, this))
	, m_sequencedDataCollection(nullptr)
	, m_softwareBrandingOptions(new SoftwareBranding)
	, m_storageAdatpterFactory(new StorageAdapterFactory)
	, m_summaryDataAccessorFactory(new SummaryDataAccessorFactory)
	, m_settings(nullptr)
	, m_translator(new QTranslator(this))
	, m_internetNotificationManager(new InternetConnectionNotificationManager(this))
	, m_webScreenShot(new WebScreenShot(this))
{
	SplashScreen::show();

	initialize();

	initializeStyleSheet();

	showMainWindow();

	INFOLOG << "Started application under OS" << operatingSystemVersion();
	INFOLOG << "Kernel type:" << QSysInfo::kernelType();
	INFOLOG << "Kernel version:" << QSysInfo::kernelVersion();
	INFOLOG << "Build ABI:" << QSysInfo::buildAbi();
	INFOLOG << "CPU:" << QSysInfo::buildCpuArchitecture();
	INFOLOG << "App Version:" << applicationVersion();
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
	DEBUGLOG << key << " " << result.toString();
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

const QPixmap& Application::crawledSitePixmap() const
{
	return m_webScreenShot->result();
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

	if (!internetAvailable())
	{
		mainWindow()->showMessageBoxDialog("Internet connection problem!",
			"It seems that you have some problems with internet connection.\n"
			"Please, check the connection and try again.",
			MessageBoxDialog::CriticalErrorIcon,
			QDialogButtonBox::Ok);

		return;
	}

	GetHostInfoRequest request(url);
	m_hostInfoRequester.reset(request, this, &Application::onHostInfoResponse);
	m_hostInfoRequester->start();
	m_webScreenShot->load(url);

	mainWindow()->statusBar()->showMessage("Checking host info...");
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

void Application::onCrawlerOptionsChanged(CrawlerEngine::CrawlerOptions options)
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
	preferences()->setMaxImageAltTextChars(options.maxImageAltTextChars);
	preferences()->setMaxH1LengthChars(options.maxH1LengthChars);
	preferences()->setMaxH2LengthChars(options.maxH2LengthChars);

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

void Application::onHostInfoResponse(CrawlerEngine::Requester* requester, const CrawlerEngine::GetHostInfoResponse& response)
{
	mainWindow()->statusBar()->clearMessage();

	m_hostInfoRequester->stop();

	if (!response.hostInfo.isValid())
	{
		mainWindow()->showMessageBoxDialog("DNS Lookup Failed!",
			"I'm sorry but I cannot find this website.\n"
			"Please, be sure that you entered a valid address.",
			MessageBoxDialog::WarningIcon,
			QDialogButtonBox::Ok);

		return;
	}

	CrawlerOptions options;

	GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());
	options.startCrawlingPage = Url(request->webpage);

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
	options.maxImageAltTextChars = preferences()->maxImageAltTextChars();
	options.maxH1LengthChars = preferences()->maxH1LengthChars();
	options.maxH2LengthChars = preferences()->maxH2LengthChars();

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

void Application::initialize()
{
	DeferredCallProcessor::init();

	m_crawler->initialize();
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

	VERIFY(connect(m_crawler, &Crawler::crawlerOptionsChanged, this, &Application::onCrawlerOptionsChanged));

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

QString Application::operatingSystemVersion()
{
	static QString osVersion;

	if (osVersion.isEmpty())
	{
#if defined(Q_OS_MACOS)
		switch (QSysInfo::MacintoshVersion)
		{
			case QSysInfo::MV_LEOPARD:
			{
				osVersion = QLatin1String("MacOS 10.5(Leopard)");
				break;
			}
			case QSysInfo::MV_TIGER:
			{
				osVersion = QLatin1String("MacOS 10.4(Tiger)");
				break;
			}
			case QSysInfo::MV_PANTHER:
			{
				osVersion = QLatin1String("MacOS 10.3(Panther)");
				break;
			}
			case QSysInfo::MV_JAGUAR:
			{
				osVersion = QLatin1String("MacOS 10.2(Jaguar)");
				break;
			}
			case QSysInfo::MV_PUMA:
			{
				osVersion = QLatin1String("MacOS 10.1(Puma)");
				break;
			}
			case QSysInfo::MV_CHEETAH:
			{
				osVersion = QLatin1String("MacOS 10.0(Cheetah)");
				break;
			}
			case QSysInfo::MV_9:
			{
				osVersion = QLatin1String("MacOS 9");
				break;
			}

			case QSysInfo::MV_Unknown:
			default:
			{
				osVersion = QLatin1String("MacOS(unknown)");
				break;
			}
		}
#elif defined(Q_OS_UNIX)
		utsname buf;
		if (uname(&buf) != -1)
		{
			osVersion.append(buf.release).append(QLatin1Char(' '));
			osVersion.append(buf.sysname).append(QLatin1Char(' '));
			osVersion.append(buf.machine).append(QLatin1Char(' '));
			osVersion.append(QLatin1String(" (")).append(buf.machine).append(QLatin1Char(')'));
		}
		else
		{
			osVersion = QLatin1String("Linux/Unix(unknown)");
		}
#elif defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
		switch (QSysInfo::WindowsVersion)
		{
			case QSysInfo::WV_CE_6:
			{
				osVersion = QLatin1String("Windows CE 6.x");
				break;
			}
			case QSysInfo::WV_CE_5:
			{
				osVersion = QLatin1String("Windows CE 5.x");
				break;
			}
			case QSysInfo::WV_CENET:
			{
				osVersion = QLatin1String("Windows CE .NET");
				break;
			}
			case QSysInfo::WV_CE:
			{
				osVersion = QLatin1String("Windows CE");
				break;
			}

			case QSysInfo::WV_WINDOWS10:
			{
				osVersion = QLatin1String("Windows 10");
				break;
			}
			case QSysInfo::WV_WINDOWS8_1:
			{
				osVersion = QLatin1String("Windows 8.1");
				break;
			}
			case QSysInfo::WV_WINDOWS8:
			{
				osVersion = QLatin1String("Windows 8");
				break;
			}
			case QSysInfo::WV_WINDOWS7:
			{
				osVersion = QLatin1String("Windows 7");
				break;
			}
			case QSysInfo::WV_VISTA:
			{
				osVersion = QLatin1String("Windows Vista");
				break;
			}
			case QSysInfo::WV_2003:
			{
				osVersion = QLatin1String("Windows Server 2003");
				break;
			}
			case QSysInfo::WV_XP:
			{
				osVersion = QLatin1String("Windows XP");
				break;
			}
			case QSysInfo::WV_2000:
			{
				osVersion = QLatin1String("Windows 2000");
				break;
			}
			case QSysInfo::WV_NT:
			{
				osVersion = QLatin1String("Windows NT");
				break;
			}

			case QSysInfo::WV_Me:
			{
				osVersion = QLatin1String("Windows Me");
				break;
			}
			case QSysInfo::WV_98:
			{
				osVersion = QLatin1String("Windows 98");
				break;
			}
			case QSysInfo::WV_95:
			{
				osVersion = QLatin1String("Windows 95");
				break;
			}
			case QSysInfo::WV_32s:
			{
				osVersion = QLatin1String("Windows 3.1 with Win32s");
				break;
			}

			default:
			{
				osVersion = QLatin1String("Windows(unknown)");
				break;
			}
		}

		if (QSysInfo::WindowsVersion & QSysInfo::WV_CE_based)
		{
			osVersion.append(QLatin1String(" (CE-based)"));
		}
		else if (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based)
		{
			osVersion.append(QLatin1String(" (NT-based)"));
		}
		else if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)
		{
			osVersion.append(QLatin1String(" (MS-DOS-based)"));
		}
#else
		return QLatin1String("Unknown");
#endif
	}

	return osVersion;
}

Application::~Application()
{
	ServiceLocator::instance()->destroyService<ISettingsPageRegistry>();
	DeferredCallProcessor::term();
}

}
