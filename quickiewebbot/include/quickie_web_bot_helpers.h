#pragma once

namespace QuickieWebBot
{

class QuickieWebBotHelpers
{
public:
	static int pointsToPixels(int dpi, double sizeInPoints);
	static double pixelsToPoints(int dpi, int sizeInPixels);
};

}