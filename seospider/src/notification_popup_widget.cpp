#include "notification_popup_widget.h"
#include "custom_push_button.h"
#include "application.h"
#include "helpers.h"

namespace SeoSpider
{

NotificationPopupWidget::NotificationPopupWidget(Status status, const QString& header, const QString& message, QWidget* relativePosition)
	: QFrame(relativePosition, Qt::Dialog | Qt::FramelessWindowHint)
	, m_parentRect(relativePosition->geometry())
	, m_borderWidth(Common::Helpers::pointsToPixels(2))
	, m_angleWidth(Common::Helpers::pointsToPixels(12))
	, m_angleHeight(Common::Helpers::pointsToPixels(10))
{
	setFocusPolicy(Qt::StrongFocus);
	setMinimumWidth(Common::Helpers::pointsToPixels(320));
	setMaximumWidth(Common::Helpers::pointsToPixels(320));
	setMinimumHeight(Common::Helpers::pointsToPixels(180));
	setMaximumHeight(Common::Helpers::pointsToPixels(180));
	setWindowModality(Qt::NonModal);

	setAttribute(Qt::WA_TranslucentBackground, true);
	setPosition();

	setContentsMargins(m_borderWidth, m_borderWidth, m_borderWidth, m_borderWidth + m_angleHeight);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* notificationTitleBarLayout = new QHBoxLayout;
	notificationTitleBarLayout->addWidget(createStatusPixmapWidget(status));
	notificationTitleBarLayout->addWidget(new QLabel(header));
	notificationTitleBarLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Expanding));
	
	QIcon closeNotificationIcon;
	closeNotificationIcon.addFile(QStringLiteral(":/images/close-notification-normal.png"), QSize(), QIcon::Normal, QIcon::On);
	closeNotificationIcon.addFile(QStringLiteral(":/images/close-notification-active.png"), QSize(), QIcon::Active, QIcon::On);
	closeNotificationIcon.addFile(QStringLiteral(":/images/close-notification-active.png"), QSize(), QIcon::Active, QIcon::Off);
	closeNotificationIcon.addFile(QStringLiteral(":/images/close-notification-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

	QToolButton* closeButton = new QToolButton(this);
	closeButton->setIcon(closeNotificationIcon);
	notificationTitleBarLayout->addWidget(closeButton);

	layout->addLayout(notificationTitleBarLayout);

	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	layout->addWidget(line);
	layout->addWidget(new QLabel(message));

	QSpacerItem* verticalSpacer = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);

	layout->addSpacerItem(verticalSpacer);

	VERIFY(connect(theApp->mainWindow(), &MainWindow::resized, this, &NotificationPopupWidget::setPosition));
	VERIFY(connect(theApp->mainWindow(), &MainWindow::moved, this, &NotificationPopupWidget::setPosition));
	VERIFY(connect(closeButton, &QToolButton::clicked, this, &NotificationPopupWidget::close));
}

void NotificationPopupWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawPixmap(rect(), m_pixmap);
}

void NotificationPopupWidget::showEvent(QShowEvent* event)
{
	const QRect adjustedRect = rect().adjusted(-m_borderWidth, -m_borderWidth, m_borderWidth, m_borderWidth + m_angleHeight);

	m_pixmap = QPixmap(adjustedRect.size());
	QPainter painter(&m_pixmap);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

	QPen pen(QColor("#D0D0D0"));
	pen.setWidth(m_borderWidth);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	const int w = width();
	const int h = height();

	const QPoint leftTopAngle = QPoint(m_borderWidth, m_borderWidth);
	const QPoint rightTopAngle(leftTopAngle.x() + w, leftTopAngle.y());
	const QPoint rightBottomAngle(leftTopAngle.x() + w, leftTopAngle.y() + h);
	const QPoint leftBottomAngle(leftTopAngle.x(), leftTopAngle.y() + h);

	QPainterPath path(leftTopAngle);
	path.lineTo(rightTopAngle);
	path.lineTo(rightBottomAngle);
	path.lineTo(rightBottomAngle.x() - w / 2, rightBottomAngle.y());
	path.lineTo(rightBottomAngle.x() - w / 2, rightBottomAngle.y() + m_angleHeight);
	path.lineTo(rightBottomAngle.x() - w / 2 - m_angleWidth, rightBottomAngle.y());
	path.lineTo(leftBottomAngle);
	painter.drawPath(path);
	painter.fillPath(path, QColor("#f2f2f2"));

	QFrame::showEvent(event);
}

QWidget* NotificationPopupWidget::createStatusPixmapWidget(Status status)
{
	QLabel* pixmapLabel = new QLabel(this);
	QPixmap pixmap(Common::Helpers::pointsToPixels(13.5), Common::Helpers::pointsToPixels(13.5));

	pixmap.fill(Qt::transparent);

	QSvgRenderer svgRenderer;
	QPainter painter(&pixmap);

	switch (status)
	{
		case Info:
		{
			svgRenderer.load(QStringLiteral(":/images/icon-ok.svg"));
			svgRenderer.render(&painter);

			break;
		}
		case Warning:
		{
			svgRenderer.load(QStringLiteral(":/images/icon-warning.svg"));
			svgRenderer.render(&painter);

			break;
		}
		case Error:
		{
			svgRenderer.load(QStringLiteral(":/images/icon-error.svg"));
			svgRenderer.render(&painter);

			break;
		}
		default:
		{
			ASSERT(!"Undefined notification status");
		}
	}

	pixmapLabel->setPixmap(pixmap);

	return pixmapLabel;
}

void NotificationPopupWidget::setPosition()
{
	const QRect appRect = theApp->mainWindow()->geometry();
	QRect thisRect = geometry();

	QPoint parentWidgetTopLeft = parentWidget()->mapToGlobal(thisRect.topLeft());
	parentWidgetTopLeft.setX(appRect.right() - thisRect.width() - Common::Helpers::pointsToPixels(10));
	parentWidgetTopLeft.setY(parentWidgetTopLeft.y() - thisRect.height() - Common::Helpers::pointsToPixels(10));

	thisRect.moveTopLeft(parentWidgetTopLeft);
	move(thisRect.topLeft());
}

}