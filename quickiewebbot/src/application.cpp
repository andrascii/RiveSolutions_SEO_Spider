#include "application.h"
#include "style_loader.h"
#include "widget_detector.h"
#include "model_controller.h"
#include "software_branding.h"
#include "start_screen.h"
#include "service_locator.h"
#include "web_crawler.h"
#include "constants.h"
#include "application_properties.h"

namespace QuickieWebBot
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_appicationProperties(new ApplicationProperties(this))
	, m_modelController(new ModelController(this))
	, m_webCrawler(new WebCrawler(g_optimalParserThreadsCount, m_modelController, this))
	, m_softwareBrandingOptions(new SoftwareBranding)
{
	initialize();

	initializeStyleSheet();

#if defined(PRODUCTION)
	showStartScreen();
#else
	mainFrameIsReadyForShow();
#endif

	INFOLOG << QThread::currentThreadId() << "Main thread has been started";

	DEBUGLOG << "Started application under OS" << operatingSystemVersion();
}

WebCrawler* Application::webCrawler() noexcept
{
	return m_webCrawler;
}

MainFrame* Application::mainFrame() noexcept
{
	return m_mainFrame.get();
}

ModelController* Application::modelController() noexcept
{
	return m_modelController;
}

QuickieWebBot::ApplicationProperties* Application::properties() noexcept
{
	return m_appicationProperties;
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

void Application::initialize() noexcept
{
	m_mainFrame.reset(new MainFrame);

#if !defined(PRODUCTION)
	StyleLoader::attachStyleLoader("styles.css", QStringLiteral("F5"));
	WidgetDetector::attachWidgetDetector(QStringLiteral("F6"));
#endif
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
			osVersion = QLatin1String("MacOS 10.5(Leopard)");
			break;
		case QSysInfo::MV_TIGER:
			osVersion = QLatin1String("MacOS 10.4(Tiger)");
			break;
		case QSysInfo::MV_PANTHER:
			osVersion = QLatin1String("MacOS 10.3(Panther)");
			break;
		case QSysInfo::MV_JAGUAR:
			osVersion = QLatin1String("MacOS 10.2(Jaguar)");
			break;
		case QSysInfo::MV_PUMA:
			osVersion = QLatin1String("MacOS 10.1(Puma)");
			break;
		case QSysInfo::MV_CHEETAH:
			osVersion = QLatin1String("MacOS 10.0(Cheetah)");
			break;
		case QSysInfo::MV_9:
			osVersion = QLatin1String("MacOS 9");
			break;

		case QSysInfo::MV_Unknown:
		default:
			osVersion = QLatin1String("MacOS(unknown)");
			break;
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
			osVersion = QLatin1String("Windows CE 6.x");
			break;
		case QSysInfo::WV_CE_5:
			osVersion = QLatin1String("Windows CE 5.x");
			break;
		case QSysInfo::WV_CENET:
			osVersion = QLatin1String("Windows CE .NET");
			break;
		case QSysInfo::WV_CE:
			osVersion = QLatin1String("Windows CE");
			break;

		case QSysInfo::WV_WINDOWS10:
			osVersion = QLatin1String("Windows 10");
			break;
		case QSysInfo::WV_WINDOWS8_1:
			osVersion = QLatin1String("Windows 8.1");
			break;
		case QSysInfo::WV_WINDOWS8:
			osVersion = QLatin1String("Windows 8");
			break;
		case QSysInfo::WV_WINDOWS7:
			osVersion = QLatin1String("Windows 7");
			break;
		case QSysInfo::WV_VISTA:
			osVersion = QLatin1String("Windows Vista");
			break;
		case QSysInfo::WV_2003:
			osVersion = QLatin1String("Windows Server 2003");
			break;
		case QSysInfo::WV_XP:
			osVersion = QLatin1String("Windows XP");
			break;
		case QSysInfo::WV_2000:
			osVersion = QLatin1String("Windows 2000");
			break;
		case QSysInfo::WV_NT:
			osVersion = QLatin1String("Windows NT");
			break;

		case QSysInfo::WV_Me:
			osVersion = QLatin1String("Windows Me");
			break;
		case QSysInfo::WV_98:
			osVersion = QLatin1String("Windows 98");
			break;
		case QSysInfo::WV_95:
			osVersion = QLatin1String("Windows 95");
			break;
		case QSysInfo::WV_32s:
			osVersion = QLatin1String("Windows 3.1 with Win32s");
			break;

		default:
			osVersion = QLatin1String("Windows(unknown)");
			break;
		}

		if (QSysInfo::WindowsVersion & QSysInfo::WV_CE_based)
			osVersion.append(QLatin1String(" (CE-based)"));
		else if (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based)
			osVersion.append(QLatin1String(" (NT-based)"));
		else if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)
			osVersion.append(QLatin1String(" (MS-DOS-based)"));
#else
		return QLatin1String("Unknown");
#endif
	}

	return osVersion;
}

void Application::showStartScreen() const noexcept
{
	StartScreen* startScreen = StartScreen::instance();

	VERIFY(connect(startScreen, &StartScreen::finished, this, &Application::mainFrameIsReadyForShow));

	startScreen->show();
}

}
