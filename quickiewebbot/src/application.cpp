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
	, m_networkAccessManager(new QNetworkAccessManager(this))
	, m_crawler(new Crawler(std::thread::hardware_concurrency(), this))
	, m_softwareBrandingOptions(new SoftwareBranding)
{
	initialize();
	m_mainFrame.reset(new MainFrame());

	initializeStyleSheet();

	showStartScreen();
}

const Crawler* Application::crawler() const noexcept
{
	return m_crawler;
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

	ServiceLocator::instance()->addService<ModelController>(new NetworkAccessManagerFutureProvider(m_networkAccessManager));
	ServiceLocator::instance()->addService<ModelController>(new ModelController);
	ServiceLocator::instance()->addService<QNetworkAccessManager>(new QNetworkAccessManager);
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
