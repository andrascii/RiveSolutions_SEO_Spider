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

}