#include "color_selector.h"

namespace SeoSpider
{

ColorSelector::ColorSelector(QWidget* parent)
	: QWidget(parent)
	, m_borderColor("#4D626E")
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

void ColorSelector::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.setBrush(QBrush(m_color));
	painter.drawRect(rect());
}

void ColorSelector::mouseReleaseEvent(QMouseEvent*)
{
	QColor selectedColor = QColorDialog::getColor();
	
	if(selectedColor == QColor())
	{
		return;
	}

	setColor(selectedColor);
}

}
