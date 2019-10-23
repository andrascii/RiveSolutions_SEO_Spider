#include "stdafx.h"
#include "custom_proxy_styles.h"

namespace SeoSpider
{

void ListItemProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	if (PE_FrameFocusRect == element)
	{
		return;
	}

	QProxyStyle::drawPrimitive(element, option, painter, widget);
}

}