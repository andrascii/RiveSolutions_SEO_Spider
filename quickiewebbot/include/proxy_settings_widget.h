#pragma once

#include "ui_proxy_settings_widget.h"

namespace QuickieWebBot
{

class ProxySettingsWidget : public QWidget
{
public:
	ProxySettingsWidget();

	Q_SLOT void onUseProxyChanged();
	Q_SLOT void onProxyNeedAuthorizationChanged();

private:
	void initialize();

private:
	Ui::ProxySettingsWidget m_ui;
};

}
