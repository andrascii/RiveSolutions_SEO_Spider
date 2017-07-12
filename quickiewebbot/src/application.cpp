#include "application.h"
#include "style_loader.h"
#include "widget_detector.h"
#include "model_controller.h"
#include "software_branding.h"
#include "start_screen.h"
#include "logger.h"

namespace QuickieWebBot
{

Application* Application::s_app = nullptr;

Application* Application::instance()
{
	return s_app;
}

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_modelController(new ModelController(this))
	, m_mainFrame(new MainFrame(m_modelController))
	, m_softwareBrandingOptions(new SoftwareBranding)
{
	initialize();

	initializeStyleSheet();

	showStartScreen();
}

MainFrame* Application::mainFrame() const noexcept
{
	return m_mainFrame.get();
}

SoftwareBranding const* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

void Application::mainFrameReadyForShow()
{
	mainFrame()->showMaximized();
}

void Application::initialize() noexcept
{
	s_app = this;
#if !defined(PRODUCTION)
	StyleLoader::attachStyleLoader("styles.css", QStringLiteral("F5"));
	WidgetDetector::attachWidgetDetector(QStringLiteral("F6"));
#endif
	INFOLOG("tag1", "text1");
	INFOLOG("tag2", "text2");
	WARNINGLOG("warning", "warning text");
	ERRORLOG("error", "error text");
	DEBUGLOG("deb", "il");
}

void Application::initializeStyleSheet() noexcept
{
	QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

	QFile styles(":/stylesheets/styles.css");

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(styles.readAll());
	}
}

void Application::showStartScreen() const noexcept
{
	StartScreen* startScreen = StartScreen::instance();

	VERIFY(connect(startScreen, &StartScreen::finished, this, &Application::mainFrameReadyForShow));

	startScreen->show();
}

}
