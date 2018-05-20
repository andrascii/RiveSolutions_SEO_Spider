#include "shaded_overlay.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider
{

ShadedOverlay::ShadedOverlay(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
}

void ShadedOverlay::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(QBrush(QColor(0, 0, 0, 150)));
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect());
}

}