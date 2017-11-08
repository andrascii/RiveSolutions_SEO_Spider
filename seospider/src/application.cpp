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

namespace SeoSpider
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_preferences(new Preferences(this, this))
	, m_crawler(new CrawlerEngine::Crawler(Common::g_optimalParserThreadsCount, this))
	, m_sequencedDataCollection(nullptr)
	, m_softwareBrandingOptions(new SoftwareBranding)
	, m_storageAdatpterFactory(new StorageAdaptorFactory)
	, m_summaryDataAccessorFactory(new SummaryDataAccessorFactory)
	, m_settings(nullptr)
	, m_translator(new QTranslator(this))
{
	SplashScreen::show();

	initialize();

	initializeStyleSheet();

	showMainWindow();

	INFOLOG << QThread::currentThreadId() << "Main thread has been started";

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

StorageAdaptorFactory* Application::storageAdaptorFactory() const noexcept
{
	return m_storageAdatpterFactory.get();
}

SummaryDataAccessorFactory* Application::summaryDataAccessorFactory() const noexcept
{
	return m_summaryDataAccessorFactory.get();
}

Preferences* Application::preferences() const noexcept
{
	return m_preferences;
}

QVariant Application::loadFromSettings(const QByteArray& key, const QVariant& defaultValue) const noexcept
{
	QVariant result = settings()->value(QLatin1String(key), defaultValue);
	DEBUGLOG << result.toString();
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

QList<QByteArray> Application::allKeys() const
{
	QList<QByteArray> result;
	
	foreach(const QString& key, settings()->allKeys())
	{
		result << key.toLatin1();
	}

	return result;
}

const SoftwareBranding* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

void Application::startCrawler()
{
	CrawlerEngine::GetHostInfoRequest request(preferences()->url().host().toLatin1());
	m_hostInfoRequester.reset(request, this, &Application::onHostInfoResponse);
	m_hostInfoRequester->start();

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

	mainWindow()->showMaximized();

	emit mainWindowShown();
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
			"I'm sorry but I cannot to find this website.\n"
			"Please, be sure that you entered a valid address.",
			MessageBoxDialog::WarningIcon,
			QDialogButtonBox::Ok);

		return;
	}

	CrawlerEngine::CrawlerOptions options;

	// preferences
	options.host = preferences()->url();
	options.minTitleLength = preferences()->minTitleLength();
	options.maxTitleLength = preferences()->maxTitleLength();
	options.limitMaxUrlLength = preferences()->limitMaxUrlLength();
	options.maxDescriptionLength = preferences()->maxDescriptionLength();
	options.minDescriptionLength = preferences()->minDescriptionLength();
	options.maxH1LengthChars = preferences()->maxH1LengthChars();
	options.maxH2LengthChars = preferences()->maxH2LengthChars();
	options.maxImageAltTextChars = preferences()->maxImageAltTextChars();
	options.maxImageSizeKb = preferences()->maxImageSize();

	// crawler settings
	options.checkExternalLinks = preferences()->checkExternalUrls();
	options.followInternalNofollow = preferences()->followInternalNoFollow();
	options.followExternalNofollow = preferences()->followExternalNoFollow();
	options.checkSubdomains = preferences()->checkSubdomains();
	options.checkImages = preferences()->checkImages();
	options.checkCss = preferences()->checkCSS();
	options.checkJavaScript = preferences()->checkJavaScript();
	options.checkSwf = preferences()->checkSWF();

	// robots.txt rules
	options.followRobotsTxtRules = theApp->preferences()->followRobotsTxt();
	options.userAgentToFollow = CrawlerEngine::UserAgentType::AnyBot;
	options.plainUserAgent = "sTechnologiesBot/1.0 Alpha (+http://www.sTechnologiesSeoSpider.org/)";

	options.parserTypeFlags.setFlag(CrawlerEngine::JavaScriptResourcesParserType);
	options.parserTypeFlags.setFlag(CrawlerEngine::CssResourcesParserType);
	options.parserTypeFlags.setFlag(CrawlerEngine::ImagesResourcesParserType);
	options.parserTypeFlags.setFlag(CrawlerEngine::VideoResourcesParserType);
	options.parserTypeFlags.setFlag(CrawlerEngine::FlashResourcesParserType);

	crawler()->startCrawling(options);
}

void Application::initialize() noexcept
{
	DeferredCallProcessor::init();

	m_crawler->initialize();
	m_sequencedDataCollection = m_crawler->sequencedDataCollection();

	SplashScreen::showMessage("Initializing...");

#ifdef PRODUCTION
	// let users to show the splash screen
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
	DebugInfoWebPageWidget::attach();
	WidgetUnderMouseInfo::attach(QStringLiteral("F6"));

#endif

	m_mainWindow->init();
}

void Application::initializeStyleSheet() noexcept
{
	SplashScreen::showMessage("Initializing stylesheets...");

	QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

	QFile styles(":/stylesheets/styles.css");

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(styles.readAll());

		INFOLOG << "Stylesheets loaded";
	}
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