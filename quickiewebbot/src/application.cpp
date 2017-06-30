#include "application.h"
#include "style_loader.h"

namespace QuickieWebBot
{

Application::Application(int argc, char** argv)
	: QApplication(argc, argv)
	, m_mainFrame(new MainFrame)
{
	initializeStyleSheet();

#if !defined(PRODUCTION)
	StyleLoader::attachStyleSheet("styles.css", QStringLiteral("F5"));
#endif

	m_mainFrame->show();
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
