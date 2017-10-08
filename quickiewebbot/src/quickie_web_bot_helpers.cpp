#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

qreal primaryScreenDpi()
{
	return QApplication::screens().at(0)->logicalDotsPerInch();
}

int QuickieWebBotHelpers::pointsToPixels(double sizeInPoints, double dpi)
{
	if (dpi == 0)
	{
		dpi = primaryScreenDpi();
	}

	const double pointSizeInPixels = dpi / 72.0;
	return qRound(sizeInPoints * pointSizeInPixels);
}

double QuickieWebBotHelpers::pixelsToPoints(int sizeInPixels, double dpi)
{
	if (dpi == 0)
	{
		dpi = primaryScreenDpi();
	}

	return (double)sizeInPixels * 72.0 / dpi;
}

void QuickieWebBotHelpers::moveWidgetToHostCenter(QWidget* widget, QWidget* host)
{
	if (!host)
	{
		INFOLOG << "Assume host as a parent";

		host = widget->parentWidget();
	}

	if (host)
	{
		INFOLOG << "Move widget to host center";

		QRect hostRect = host->geometry();
		widget->move(hostRect.center() - widget->rect().center());

		return;
	}

	INFOLOG << "Move widget to screen center";

	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width() - widget->width()) / 2;
	int y = (screenGeometry.height() - widget->height()) / 2;
	widget->move(x, y);
}

}