#include "application.h"
#include "style_loader.h"
#include "model_controller.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "service_locator.h"
#include "web_crawler.h"
#include "gui_storage.h"
#include "constants.h"
#include "application_settings.h"
#include "debug_info_web_page_widget.h"
#include "settings_page_impl.h"

#include "ui_crawler_settings.h"
#include "ui_proxy_settings.h"
#include "ui_limits_settings.h"
#include "ui_preferences_settings.h"

namespace QuickieWebBot
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_appicationProperties(new ApplicationSettings(this))
	, m_webCrawler(new WebCrawler::WebCrawler(WebCrawler::g_optimalParserThreadsCount))
	, m_softwareBrandingOptions(new SoftwareBranding)
	, m_storageAdatpterFactory(new StorageAdaptorFactory)
	, m_summaryDataAccessorFactory(new SummaryDataAccessorFactory)
{
	WebCrawler::GuiStorage* storage = m_webCrawler->guiStorage();
	ASSERT(storage->thread() == QThread::currentThread());
	m_guiStorage = storage;

	QFile config(":/config/defaults.cfg");
	config.open(QIODevice::ReadOnly);
	m_appicationProperties->setDefaults(config.readAll());
	loadFromSettings();

	initialize();

	initializeStyleSheet();

#if defined(PRODUCTION)
	showSplashScreen();
#else
	mainFrameIsReadyForShow();
#endif

	INFOLOG << QThread::currentThreadId() << "Main thread has been started";

	INFOLOG << "Started application under OS" << operatingSystemVersion();
	INFOLOG << "Kernel type:" << QSysInfo::kernelType();
	INFOLOG << "Kernel version:" << QSysInfo::kernelVersion();
	INFOLOG << "Build ABI:" << QSysInfo::buildAbi();
	INFOLOG << "CPU:" << QSysInfo::buildCpuArchitecture();
}

Application::~Application()
{

}

WebCrawler::WebCrawler* Application::webCrawler() noexcept
{
	return m_webCrawler.get();
}

MainFrame* Application::mainFrame() noexcept
{
	return m_mainFrame.get();
}

WebCrawler::GuiStorage* Application::guiStorage() noexcept
{
	return m_guiStorage;
}

StorageAdaptorFactory* Application::storageAdaptorFactory() noexcept
{
	return m_storageAdatpterFactory.get();
}

SummaryDataAccessorFactory* Application::summaryDataAccessorFactory() noexcept
{
	return m_summaryDataAccessorFactory.get();
}

ApplicationSettings* Application::properties() noexcept
{
	return m_appicationProperties;
}

void Application::loadFromSettings() noexcept
{
	const QMetaObject* metaObject = theApp->properties()->metaObject();
	int propertyCount = metaObject->propertyCount();

	for (int i = 0; i < propertyCount; i++)
	{
		QMetaProperty metaPropertry = metaObject->property(i);
		const char* metaPropertryName = metaPropertry.name();
		theApp->properties()->setProperty(metaPropertryName, m_appicationProperties->get(metaPropertryName));
	}
}

void Application::saveToSettings() noexcept
{
	const QMetaObject* metaObject = theApp->properties()->metaObject();
	int propertyCount = metaObject->propertyCount();

	for (int i = 0; i < propertyCount; i++)
	{
		QMetaProperty metaPropertry = metaObject->property(i);
		const char* metaPropertryName = metaPropertry.name();
		m_appicationProperties->set(metaPropertryName, theApp->properties()->property(metaPropertryName));
	}
}

const SoftwareBranding* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

void Application::mainFrameIsReadyForShow()
{
	mainFrame()->showMaximized();

	INFOLOG << "MainFrame shown";
}

void Application::registerSettingsPages() const
{
	SettingsPageImpl<Ui_CrawlerSettings>::registerSettingsPage(QIcon(":/images/crawler-settings.png"), TYPE_STRING(Ui_CrawlerSettings));
	SettingsPageImpl<Ui_ProxySettings>::registerSettingsPage(QIcon(":/images/proxy-settings.png"), TYPE_STRING(Ui_ProxySettings));
	SettingsPageImpl<Ui_LimitsSettings>::registerSettingsPage(QIcon(":/images/limits-settings.png"), TYPE_STRING(Ui_LimitsSettings));
	SettingsPageImpl<Ui_PreferencesSettings>::registerSettingsPage(QIcon(":/images/preferences-settings-icon.png"), TYPE_STRING(Ui_PreferencesSettings));
}

void Application::registerServices() const
{
	ServiceLocator::instance()->addService<SettingsPageRegistry>(new SettingsPageRegistry);
}

void Application::initialize() noexcept
{
	registerServices();

	m_mainFrame.reset(new MainFrame);

#if !defined(PRODUCTION)
	StyleLoader::attachStyleLoader("styles.css", QStringLiteral("F5"));
	DebugInfoWebPageWidget::attachDebugInfoWebPageWidget();
#endif

	registerSettingsPages();
}

void Application::initializeStyleSheet() noexcept
{
	QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

	QFile styles(":/stylesheets/styles.css");

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(styles.readAll());

		INFOLOG << "Stylesheets loaded";
	}
}

QString Application::operatingSystemVersion() const noexcept
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

void Application::showSplashScreen() const noexcept
{
	SplashScreen* splashScreen = SplashScreen::instance();

	VERIFY(connect(splashScreen, &SplashScreen::finished, this, &Application::mainFrameIsReadyForShow));

	splashScreen->show();
}

}
