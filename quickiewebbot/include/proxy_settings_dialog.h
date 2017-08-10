#pragma once

#include "ui_proxy_settings.h"

namespace QuickieWebBot
{

class ProxySettingsDialog : public QDialog
{
	Q_OBJECT

public:
	ProxySettingsDialog(QWidget* parent = nullptr);

	Q_SLOT void onUseProxyChanged(int state);
	Q_SLOT void onProxyNeedAuthorizationChanged(int state);

private:
	void initialize();

private:
	Ui::ProxySettingsDialog m_ui;
};

}
