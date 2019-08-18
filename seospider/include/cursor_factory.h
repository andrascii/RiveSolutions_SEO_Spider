#pragma once

namespace SeoSpider
{

class CursorFactory final
{
public:
	static QCursor createCursor(Qt::CursorShape shape);
};

}