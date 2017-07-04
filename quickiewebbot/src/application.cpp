#include "application.h"
#include "style_loader.h"
#include "widget_detector.h"
#include "model_controller.h"
#include "service_locator.h"
#include "thread_dispatcher.h"

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
{
	initialize();

	initializeStyleSheet();

	m_mainFrame->show();
}

MainFrame const* Application::mainFrame() const noexcept
{
	return m_mainFrame.get();
}

void Application::initialize()
{
#if !defined(PRODUCTION)
	StyleLoader::attachStyleLoader("styles.css", QStringLiteral("F5"));
	WidgetDetector::attachWidgetDetector(QStringLiteral("F6"));
#endif

	s_app = this;

	ServiceLocator::instance()->addService<QNetworkAccessManager>(new QNetworkAccessManager);
}

void Application::initializeStyleSheet()
{
	QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

	QFile styles(":/stylesheets/styles.css");

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(styles.readAll());
	}
}

}
