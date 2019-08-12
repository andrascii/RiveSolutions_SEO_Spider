#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class CursorFactory final
{
public:
	static QCursor createCursor(Qt::CursorShape shape);
};

}