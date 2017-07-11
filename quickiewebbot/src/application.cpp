#include "application.h"
#include "style_loader.h"
#include "widget_detector.h"
#include "model_controller.h"
#include "software_branding.h"
#include "start_screen.h"
#include "service_locator.h"
#include "crawler.h"
#include "network_access_manager_future_provider.h"

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
	, m_crawler(new Crawler(std::thread::hardware_concurrency(), m_modelController, this))
	, m_downloader(new Downloader)
	, m_softwareBrandingOptions(new SoftwareBranding)
{
	s_app = this;

	initialize();

	m_downloader->scheduleRequest(QUrl("http://www.cyberforum.ru"));

	initializeStyleSheet();

#if defined(PRODUCTION)
	showStartScreen();
#else
	mainFrameReadyForShow();
#endif
}

const Crawler* Application::crawler() const noexcept
{
	return m_crawler;
}

MainFrame* Application::mainFrame() noexcept
{
	return m_mainFrame.get();
}

ModelController* Application::modelController() noexcept
{
	return m_modelController;
}

const SoftwareBranding* Application::softwareBrandingOptions() const noexcept
{
	return m_softwareBrandingOptions.get();
}

void Application::mainFrameReadyForShow()
{
	mainFrame()->showMaximized();
}

void Application::initialize() noexcept
{
	m_mainFrame.reset(new MainFrame);

#if !defined(PRODUCTION)
	StyleLoader::attachStyleLoader("styles.css", QStringLiteral("F5"));
	WidgetDetector::attachWidgetDetector(QStringLiteral("F6"));
#endif

	ServiceLocator::instance()->addService<NetworkAccessManagerFutureProvider>(new NetworkAccessManagerFutureProvider);

	m_downloader->start();
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
