#include "color_selector.h"
#include "helpers.h"

namespace SeoSpider
{

ColorSelector::ColorSelector(QWidget* parent)
	: QWidget(parent)
	, m_borderWidth(Common::Helpers::pointsToPixels(2))
	, m_borderColor("#4D626E")
{
}

QColor ColorSelector::getColor() const
{
	return m_color;
}

const QColor& ColorSelector::borderColor() const noexcept
{
	return m_borderColor;
}

void ColorSelector::setBorderColor(const QColor& color) noexcept
{
	m_borderColor = color;

	emit borderColorChanged();
}

void ColorSelector::setColor(QColor value)
{
	m_color = value;
	emit colorChanged();
}

void ColorSelector::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(QPen(m_borderColor, m_borderWidth, Qt::SolidLine));
	painter.setBrush(QBrush(m_color));

	painter.drawRect(rect());
}

void ColorSelector::mouseReleaseEvent(QMouseEvent* event)
{
	if(event->button() != Qt::LeftButton)
	{
		return;
	}

	QColor selectedColor = QColorDialog::getColor(m_color);
	
	if(selectedColor == QColor() || selectedColor == m_color)
	{
		return;
	}

	setColor(selectedColor);
}

}
