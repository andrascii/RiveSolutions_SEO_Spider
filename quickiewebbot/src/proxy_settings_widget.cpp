#include "application.h"
#include "application_properties.h"
#include "proxy_settings_widget.h"

namespace QuickieWebBot
{

ProxySettingsWidget::ProxySettingsWidget()
	: QWidget(nullptr)
{
	initialize();
}

void ProxySettingsWidget::onUseProxyChanged()
{
	theApp->properties()->setUseProxy(m_ui.useProxyCheckBox->isChecked());
}

void ProxySettingsWidget::onProxyNeedAuthorizationChanged()
{

}

void ProxySettingsWidget::initialize()
{
	m_ui.setupUi(this);

	//VERIFY(connect(m_ui.useProxyCheckBox, SIGNAL(clicked(bool)), SLOT(onUseProxyChanged())));
}

}