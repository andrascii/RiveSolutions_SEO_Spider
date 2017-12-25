#include "helpers.h"

namespace
{

qreal primaryScreenDpi()
{
	return QApplication::screens().at(0)->logicalDotsPerInch();
}

}

namespace Common
{

int Helpers::pointsToPixels(double sizeInPoints, double dpi)
{
	if (dpi == 0)
	{
		dpi = primaryScreenDpi();
	}

	const double pointSizeInPixels = dpi / 72.0;
	return qRound(sizeInPoints * pointSizeInPixels);
}

double Helpers::pixelsToPoints(int sizeInPixels, double dpi)
{
	if (dpi == 0)
	{
		dpi = primaryScreenDpi();
	}

	return static_cast<double>(sizeInPixels) * 72.0 / dpi;
}

void Helpers::moveWidgetToHostCenter(QWidget* widget, QWidget* host)
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

	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	const int x = (screenGeometry.width() - widget->width()) / 2;
	const int y = (screenGeometry.height() - widget->height()) / 2;
	widget->move(x, y);
}

}