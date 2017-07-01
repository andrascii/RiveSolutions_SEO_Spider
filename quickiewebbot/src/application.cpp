#include "application.h"
#include "style_loader.h"
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
	, m_mainFrame(new MainFrame)
{
	initialize();

	initializeStyleSheet();

	m_mainFrame->show();
}

void Application::initialize()
{
#if !defined(PRODUCTION)
	StyleLoader::attachStyleSheet("styles.css", QStringLiteral("F5"));
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
