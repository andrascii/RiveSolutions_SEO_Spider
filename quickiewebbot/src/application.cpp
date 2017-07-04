#include "application.h"
#include "style_loader.h"
#include "widget_detector.h"
#include "model_controller.h"
#include "start_screen.h"

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
	, m_mainFrame(new MainFrame)
	, m_softwareBrandingOptions(new SoftwareBranding)
{
	/* initialize debug features if needed and myApp macro */
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

void Application::mainFrameIsReadyToShow()
{
	mainFrame()->show();
}

void Application::initialize() noexcept
{
	s_app = this;

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
	}
}

void Application::showStartScreen() const noexcept
{
	StartScreen* startScreen = StartScreen::instance();

	VERIFY(connect(startScreen, &StartScreen::finished, this, &Application::mainFrameIsReadyToShow));

	startScreen->show();
}

}
