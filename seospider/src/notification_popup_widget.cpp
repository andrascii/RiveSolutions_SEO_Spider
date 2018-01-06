#include "notification_popup_widget.h"
#include "helpers.h"

namespace SeoSpider
{

NotificationPopupWidget::NotificationPopupWidget(Status status, const QString& header, const QString& message, QWidget* relativePosition)
	: QWidget(relativePosition, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
	, m_status(status)
	, m_header(header)
	, m_message(message)
	, m_parentRect(relativePosition->geometry())
{
	setMinimumWidth(Common::Helpers::pointsToPixels(320));
	setMaximumWidth(Common::Helpers::pointsToPixels(320));
	setMinimumHeight(Common::Helpers::pointsToPixels(190));
	setMaximumHeight(Common::Helpers::pointsToPixels(190));

	setWindowModality(Qt::NonModal);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new QPushButton("button"));

	const QRect desktop = QApplication::desktop()->geometry();
	QRect thisRect = geometry();

	QPoint parentWidgetTopLeft = parentWidget()->mapToGlobal(thisRect.topLeft());
	parentWidgetTopLeft.setX(desktop.right() - thisRect.width() - Common::Helpers::pointsToPixels(10));
	parentWidgetTopLeft.setY(parentWidgetTopLeft.y() - thisRect.height() - Common::Helpers::pointsToPixels(5));

	thisRect.moveTopLeft(parentWidgetTopLeft);
	move(thisRect.topLeft());
}

void NotificationPopupWidget::setStatus(Status status)
{
	m_status = status;
}

void NotificationPopupWidget::setHeader(const QString& header)
{
	m_header = header;
}

void NotificationPopupWidget::setMessage(const QString& message)
{
	m_message = message;
}

void NotificationPopupWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	QPen pen(QColor("#D6D6D6"));
	pen.setWidth(Common::Helpers::pointsToPixels(2));
	painter.setPen(pen);

	const QRect adjustedRect = rect().adjusted(1, 1, -1, -1);
	const QPoint leftTopAngle = QPoint(adjustedRect.x(), adjustedRect.y());
	const QPoint rightTopAngle(leftTopAngle.x() + width(), leftTopAngle.y());
	const QPoint rightBottomAngle(leftTopAngle.x() + width(), leftTopAngle.y() + height());
	const QPoint leftBottomAngle(leftTopAngle.x(), leftTopAngle.y() + height());

	QPainterPath path(leftTopAngle);
	path.lineTo(rightTopAngle);
	path.lineTo(rightBottomAngle);
	path.lineTo(leftBottomAngle);

	painter.save();
	painter.drawPath(path);
	painter.restore();
}

}