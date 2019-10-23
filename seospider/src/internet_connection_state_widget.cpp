#include "stdafx.h"
#include "internet_connection_state_widget.h"
#include "internet_connection_notification_manager.h"
#include "application.h"

namespace SeoSpider
{

InternetConnectionStateWidget::InternetConnectionStateWidget(QWidget* parent)
	: QWidget(parent)
	, m_pixmapLabel(new QLabel(this))
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_pixmapLabel);

	VERIFY(connect(theApp->internetConnectionNotificationManager(), &InternetConnectionNotificationManager::internetConnectionStateChanged,
		this, &InternetConnectionStateWidget::onInternetConnectionStateChanged));

	onInternetConnectionStateChanged();
}

void InternetConnectionStateWidget::onInternetConnectionStateChanged()
{
	m_pixmapLabel->setPixmap(internetConnectionStatePixmap());
	m_pixmapLabel->setToolTip(internetConnectionStateToolTip());
}

QPixmap InternetConnectionStateWidget::internetConnectionStatePixmap() const
{
	if (theApp->internetConnectionNotificationManager()->internetAvailable())
	{
		return QPixmap(QStringLiteral(":/images/internet-connection.png"));
	}
	else
	{
		return QPixmap(QStringLiteral(":/images/no-internet-connection.png"));
	}
}

QString InternetConnectionStateWidget::internetConnectionStateToolTip() const
{
	if (theApp->internetConnectionNotificationManager()->internetAvailable())
	{
		return tr("Has internet connection");
	}
	else
	{
		return tr("Has no internet connection");
	}
}

}