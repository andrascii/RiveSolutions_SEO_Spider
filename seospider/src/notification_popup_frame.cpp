#include "notification_popup_frame.h"
#include "custom_push_button.h"
#include "application.h"
#include "helpers.h"
#include "svg_renderer.h"

namespace SeoSpider
{

NotificationPopupFrame::NotificationPopupFrame(Status status, const QString& header, const QString& message, QWidget* relativePosition)
	: QFrame(relativePosition, Qt::Dialog | Qt::FramelessWindowHint)
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

	QPixmap normalClosePixmap = SvgRenderer::render(QStringLiteral(":/images/close-notification.svg"), 6, 6);
	QIcon closeNotificationIcon;
	closeNotificationIcon.addPixmap(normalClosePixmap, QIcon::Normal, QIcon::On);

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

	adjustSize();
	setPosition();

	VERIFY(connect(closeButton, &QToolButton::clicked, this, &NotificationPopupFrame::close));
	VERIFY(connect(theApp->mainWindow(), &MainWindow::resized, this, &NotificationPopupFrame::setPosition));
	VERIFY(connect(theApp->mainWindow(), &MainWindow::moved, this, &NotificationPopupFrame::setPosition));
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

	QPixmap pixmap;
	const QSize size(13.5, 13.5);

	switch (status)
	{
		case Info:
		{
			pixmap = SvgRenderer::render(QStringLiteral(":/images/icon-ok.svg"), size);
			break;
		}
		case Warning:
		{
			pixmap = SvgRenderer::render(QStringLiteral(":/images/icon-warning.svg"), size);
			break;
		}
		case Error:
		{
			pixmap = SvgRenderer::render(QStringLiteral(":/images/icon-error.svg"), size);
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

	const QPoint parentWidgetTopLeftGlobalPoint = parentWidget()->mapToGlobal(QPoint(0, 0));
	const QPoint parentWidgetTopLeftMappedToThisPoint = mapFromGlobal(parentWidgetTopLeftGlobalPoint);

	const QRect parentRect = parentWidget()->geometry();
	const int point = Common::Helpers::pointsToPixels(1);
	const int parentWidgetCenter = point + m_borderWidth + parentWidgetTopLeftMappedToThisPoint.x() + parentRect.width() / 2;

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