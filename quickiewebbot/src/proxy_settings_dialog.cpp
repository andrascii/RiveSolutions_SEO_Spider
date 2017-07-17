#include "application.h"
#include "application_properties.h"
#include "proxy_settings_dialog.h"

namespace QuickieWebBot
{

ProxySettingsDialog::ProxySettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	initialize();
}

void ProxySettingsDialog::onUseProxyChanged(int state)
{
	theApp->properties()->setUseProxy(state == Qt::Checked);
}

void ProxySettingsDialog::onProxyNeedAuthorizationChanged(int state)
{
	theApp->properties()->setProxyNeedAuthorization(state == Qt::Checked);
}

void ProxySettingsDialog::initialize()
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.useProxyCheckBox, SIGNAL(stateChanged(int)), SLOT(onUseProxyChanged(int))));
	VERIFY(connect(m_ui.proxyNeedAuthorizationCheckBox, SIGNAL(stateChanged(int)), SLOT(onProxyNeedAuthorizationChanged(int))));
}

}