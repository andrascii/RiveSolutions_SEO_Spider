#include "proxy_settings_widget.h"

namespace SeoSpider
{

ProxySettingsWidget::ProxySettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	init();
}

bool ProxySettingsWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_ui.useProxyLabel && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.useProxyCheckBox->toggle();
	}

	if (object == m_ui.proxyRequiresAuthorizationLabel && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.proxyNeedAuthorizationCheckBox->toggle();
	}

	return false;
}

void ProxySettingsWidget::init()
{
	m_ui.useProxyLabel->installEventFilter(this);
	m_ui.proxyRequiresAuthorizationLabel->installEventFilter(this);

	SettingsPage::init();
}

}