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

#ifdef _WINDOWS
	INFOLOG("", QString::number(reinterpret_cast<DWORD>(QThread::currentThreadId())) + " Main Thread has been started");
#endif
}

const WebCrawler* Application::webCrawler() const noexcept
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

	INFOLOG("", "MainFrame shown");
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

		INFOLOG("", "Stylesheets loaded");
	}
}

void Application::showStartScreen() const noexcept
{
	StartScreen* startScreen = StartScreen::instance();

	VERIFY(connect(startScreen, &StartScreen::finished, this, &Application::mainFrameIsReadyForShow));

	startScreen->show();
}

}
