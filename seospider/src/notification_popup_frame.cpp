#include "notification_popup_frame.h"
#include "custom_push_button.h"
#include "application.h"
#include "helpers.h"

namespace SeoSpider
{

NotificationPopupFrame::NotificationPopupFrame(Status status, const QString& header, const QString& message, QWidget* relativePosition)
	: QFrame(relativePosition, Qt::Dialog | Qt::FramelessWindowHint)
	, m_parentRect(relativePosition->geometry())
	, m_borderWidth(Common::Helpers::pointsToPixels(2))
	, m_angleWidth(Common::Helpers::pointsToPixels(12))
	, m_angleHeight(Common::Helpers::pointsToPixels(10))
	, m_borderColor("#4D626E")
	, m_backgroundColor("#F2F2F2")
{
	QSize minimumSize(Common::Helpers::pointsToPixels(250), Common::Helpers::pointsToPixels(120));
	setMinimumSize(minimumSize);

	setFocusPolicy(Qt::StrongFocus);
	setWindowModality(Qt::NonModal);
	setAttribute(Qt::WA_TranslucentBackground, true);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QLabel* headerLabel = new QLabel(header, this);
	headerLabel->setObjectName("NotificationHeaderLabel");

	QHBoxLayout* notificationTitleBarLayout = new QHBoxLayout;
	notificationTitleBarLayout->addWidget(createStatusPixmapWidget(status));
	notificationTitleBarLayout->addWidget(headerLabel);
	notificationTitleBarLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Expanding));

	QIcon closeNotificationIcon;
	QSize closePixmapSize(Common::Helpers::pointsToPixels(6), Common::Helpers::pointsToPixels(6));
	QPixmap activeClosePixmap(closePixmapSize);
	QPixmap normalClosePixmap(closePixmapSize);
	QSvgRenderer renderer;
	QPainter painterActive(&activeClosePixmap);
	QPainter painterNormal(&normalClosePixmap);

	renderer.load(QStringLiteral(":/images/close-notification-active.svg"));
	renderer.render(&painterActive);

	renderer.load(QStringLiteral(":/images/close-notification-normal.svg"));
	renderer.render(&painterNormal);

	closeNotificationIcon.addPixmap(normalClosePixmap.scaled(closePixmapSize), QIcon::Normal, QIcon::On);
	closeNotificationIcon.addPixmap(activeClosePixmap.scaled(closePixmapSize), QIcon::Active, QIcon::On);
	closeNotificationIcon.addPixmap(activeClosePixmap.scaled(closePixmapSize), QIcon::Active, QIcon::Off);
	closeNotificationIcon.addPixmap(normalClosePixmap.scaled(closePixmapSize), QIcon::Normal, QIcon::Off);

	QToolButton* closeButton = new QToolButton(this);
	closeButton->setIcon(closeNotificationIcon);
	notificationTitleBarLayout->addWidget(closeButton);

	layout->addLayout(notificationTitleBarLayout);

	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	layout->addWidget(line);
	layout->addWidget(new QLabel(message));

	QSpacerItem* verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

	layout->addSpacerItem(verticalSpacer);

	VERIFY(connect(theApp->mainWindow(), &MainWindow::resized, this, &NotificationPopupFrame::setPosition));
	VERIFY(connect(theApp->mainWindow(), &MainWindow::moved, this, &NotificationPopupFrame::setPosition));
	VERIFY(connect(closeButton, &QToolButton::clicked, this, &NotificationPopupFrame::close));

	adjustSize();
	setPosition();

	VERIFY(connect(this, &NotificationPopupFrame::borderColorChanged, this, &NotificationPopupFrame::redrawPixmap));
	VERIFY(connect(this, &NotificationPopupFrame::backgroundColorChanged, this, &NotificationPopupFrame::redrawPixmap));
}

const QColor& NotificationPopupFrame::borderColor() const noexcept
{
	return m_borderColor;
}

void NotificationPopupFrame::setBorderColor(const QColor& color) noexcept
{
	m_borderColor = color;

	emit borderColorChanged();
}

const QColor& NotificationPopupFrame::backgroundColor() const noexcept
{
	return m_backgroundColor;
}

void NotificationPopupFrame::setBackgroundColor(const QColor& color) noexcept
{
	m_backgroundColor = color;

	emit backgroundColorChanged();
}

void NotificationPopupFrame::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawPixmap(rect(), m_pixmap);
}

void NotificationPopupFrame::showEvent(QShowEvent* event)
{
	redrawPixmap();
	QFrame::showEvent(event);
}

QWidget* NotificationPopupFrame::createStatusPixmapWidget(Status status)
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

void NotificationPopupFrame::setPosition()
{
	const QRect appRect = theApp->mainWindow()->geometry();
	QPoint parentWidgetTopLeft = parentWidget()->mapToGlobal(QPoint(0, 0));
	parentWidgetTopLeft.setX(appRect.right() - width() - Common::Helpers::pointsToPixels(10));
	parentWidgetTopLeft.setY(parentWidgetTopLeft.y() - height() + Common::Helpers::pointsToPixels(5));
	move(parentWidgetTopLeft);
}

void NotificationPopupFrame::redrawPixmap()
{
	const QRect adjustedRect = rect().adjusted(-m_borderWidth, -m_borderWidth, m_borderWidth, m_borderWidth + m_angleHeight);
	const QRect adjustedRectBorder = adjustedRect.adjusted(m_borderWidth, m_borderWidth, -m_borderWidth, -m_borderWidth);

	m_pixmap = QPixmap(adjustedRect.size());
	m_pixmap.fill(Qt::transparent);
	QPainter painter(&m_pixmap);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

	QPen pen(borderColor());
	pen.setWidth(m_borderWidth);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	const int w = width();
	const int h = height();

	const QPoint leftTopAngle = QPoint(m_borderWidth, m_borderWidth);
	const QPoint rightTopAngle(leftTopAngle.x() + w - m_borderWidth, leftTopAngle.y());
	const QPoint rightBottomAngle(leftTopAngle.x() + w - m_borderWidth, leftTopAngle.y() + h - m_borderWidth);
	const QPoint leftBottomAngle(leftTopAngle.x(), leftTopAngle.y() + h - m_borderWidth);

	const int parentWidgetTopLeft = mapFromParent(m_parentRect.topLeft()).x();
	const int parentWidgetCenter = Common::Helpers::pointsToPixels(1) + m_borderWidth + parentWidgetTopLeft + m_parentRect.width() / 2;

	QPainterPath path(leftTopAngle);
	path.lineTo(rightTopAngle);
	path.lineTo(rightBottomAngle);
	path.lineTo(parentWidgetCenter, rightBottomAngle.y());
	path.lineTo(parentWidgetCenter, rightBottomAngle.y() + m_angleHeight);
	path.lineTo(parentWidgetCenter - m_angleWidth, rightBottomAngle.y());
	path.lineTo(leftBottomAngle);
	path.lineTo(leftTopAngle);

	painter.drawPath(path);
	painter.fillPath(path, backgroundColor());
}

}