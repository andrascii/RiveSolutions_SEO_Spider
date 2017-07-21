#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

int QuickieWebBotHelpers::pointsToPixels(int dpi, double sizeInPoints)
{
	const double pointSizeInPixels = dpi / 72.0;
	return qRound(sizeInPoints * pointSizeInPixels);
}

double QuickieWebBotHelpers::pixelsToPoints(int dpi, int sizeInPixels)
{
	return (double)sizeInPixels * 72.0 / dpi;
}

}