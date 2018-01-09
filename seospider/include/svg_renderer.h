#pragma once

namespace SeoSpider
{

class SvgRenderer
{
public:
	static QPixmap render(const QString& filepath, const QSize& sizeInPoints);
	static QPixmap render(const QString& filepath, int widthInPoints, int heightInPoints);
};

}