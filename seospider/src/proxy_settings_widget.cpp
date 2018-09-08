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
	if (object == m_ui.useProxyLabel &&
		event->type() == QEvent::MouseButtonRelease &&
		m_ui.useProxyCheckBox->isEnabled())
	{
		m_ui.useProxyCheckBox->toggle();
	}

	if (object == m_ui.proxyRequiresAuthorizationLabel &&
		event->type() == QEvent::MouseButtonRelease &&
		m_ui.proxyNeedAuthorizationCheckBox->isEnabled())
	{
		m_ui.proxyNeedAuthorizationCheckBox->toggle();
	}

	return false;
}

void ProxySettingsWidget::init()
{
	m_ui.useProxyLabel->installEventFilter(this);
	m_ui.proxyRequiresAuthorizationLabel->installEventFilter(this);

	m_ui.proxyAddressLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.portSpinBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.proxyNeedAuthorizationCheckBox->setEnabled(m_ui.useProxyCheckBox->isChecked());

	m_ui.usernameLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.passwordLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());

	SettingsPage::init();

	VERIFY(connect(m_ui.useProxyCheckBox, SIGNAL(stateChanged(int)), SLOT(onCheckBoxToggled())));
	VERIFY(connect(m_ui.proxyNeedAuthorizationCheckBox, SIGNAL(stateChanged(int)), SLOT(onCheckBoxToggled())));
}

void ProxySettingsWidget::onCheckBoxToggled()
{
	m_ui.proxyAddressLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.portSpinBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.proxyNeedAuthorizationCheckBox->setEnabled(m_ui.useProxyCheckBox->isChecked());
	m_ui.usernameLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
	m_ui.passwordLineEdit->setEnabled(m_ui.useProxyCheckBox->isChecked() && m_ui.proxyNeedAuthorizationCheckBox->isChecked());
}

void ProxySettingsWidget::onShow()
{
}

void ProxySettingsWidget::onClose()
{
}

}