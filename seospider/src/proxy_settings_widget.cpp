#include "stdafx.h"
#include "proxy_settings_widget.h"

namespace SeoSpider
{

ProxySettingsWidget::ProxySettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	init();
}

void ProxySettingsWidget::init()
{
	m_ui.proxyAddressLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.portSpinBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.proxyNeedAuthorizationCheckBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.proxyAddressLabel->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.portLabel->setEnabled(m_ui.useProxyCheckBox->isChecked());

	m_ui.usernameLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.passwordLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.usernameLabel->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.passwordLabel->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());

	SettingsPage::init();

	VERIFY(connect(m_ui.useProxyCheckBox, SIGNAL(stateChanged(int)), SLOT(onCheckBoxToggled())));
	VERIFY(connect(m_ui.proxyNeedAuthorizationCheckBox, SIGNAL(stateChanged(int)), SLOT(onCheckBoxToggled())));
}

void ProxySettingsWidget::onCheckBoxToggled()
{
	m_ui.proxyAddressLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.proxyAddressLabel->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.portLabel->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.portSpinBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.proxyNeedAuthorizationCheckBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.usernameLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.passwordLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.usernameLabel->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.passwordLabel->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
}

void ProxySettingsWidget::onShow()
{
}

void ProxySettingsWidget::onClose()
{
}

}