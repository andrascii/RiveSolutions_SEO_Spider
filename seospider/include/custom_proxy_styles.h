#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class ListItemProxyStyle : public QProxyStyle
{
public:
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = nullptr) const override;
};

}