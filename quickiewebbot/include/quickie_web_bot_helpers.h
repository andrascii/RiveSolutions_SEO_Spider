#pragma once

namespace QuickieWebBot
{

class QuickieWebBotHelpers
{
public:
	static int pointsToPixels(double sizeInPoints, double dpi = 0);
	static double pixelsToPoints(int sizeInPixels, double dpi = 0);
};

}