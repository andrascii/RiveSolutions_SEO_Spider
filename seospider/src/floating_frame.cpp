#include "floating_frame.h"
#include "cursor_factory.h"

namespace SeoSpider
{

FloatingFrame::FloatingFrame(QWidget* parent)
	: QFrame(parent)
{
	m_mousePressed = false;

	setCursor(CursorFactory::createCursor(Qt::OpenHandCursor));
}

void FloatingFrame::mousePressEvent(QMouseEvent* event)
{
	setCursor(CursorFactory::createCursor(Qt::ClosedHandCursor));

	m_mousePressed = true;
	m_mousePos = event->globalPos();
	m_wndPos = pos();
}

void FloatingFrame::mouseMoveEvent(QMouseEvent* event)
{
	if (m_mousePressed)
	{
		move(m_wndPos + (event->globalPos() - m_mousePos));
	}
}

void FloatingFrame::mouseReleaseEvent(QMouseEvent*)
{
	m_mousePressed = false;

	setCursor(CursorFactory::createCursor(Qt::OpenHandCursor));
}

void FloatingFrame::paintEvent(QPaintEvent*)
{
	QStyleOption styleOption;
	styleOption.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
}

}