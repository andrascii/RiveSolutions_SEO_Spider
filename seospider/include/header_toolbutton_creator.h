#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class HeaderToolButtonCreator final
{
public:
	static QWidget* createControl(QAction* action);
};

}