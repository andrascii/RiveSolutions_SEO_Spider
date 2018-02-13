#include "color_selector.h"

namespace SeoSpider
{

ColorSelector::ColorSelector(QObject* parent)
	: QObject(parent)
{
}

QColor ColorSelector::getColor() const
{
	return m_color;
}

void ColorSelector::setColor(QColor value)
{
	m_color = value;
	emit colorChanged();
}

}