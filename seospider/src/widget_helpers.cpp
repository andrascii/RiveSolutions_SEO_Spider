#include "stdafx.h"
#include "widget_helpers.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider::WidgetHelpers
{

void moveWidgetToHostCenter(QWidget* widget, QWidget* host)
{
	if (!host)
	{
		host = widget->parentWidget();
	}

	if (host)
	{
		QRect hostRect = host->geometry();
		widget->move(hostRect.center() - widget->rect().center());

		return;
	}

	QDesktopWidget* desktopWidget = QApplication::desktop();

	const QRect screenGeometry = desktopWidget->screenGeometry(theApp->mainWindow());
	
	const int sign = screenGeometry.x() ? screenGeometry.x() / -screenGeometry.x() : 1;
	const int x = (screenGeometry.width() * sign - widget->width()) / 2;
	const int y = (screenGeometry.height() - widget->height()) / 2;

	widget->move(x, y);
}

}