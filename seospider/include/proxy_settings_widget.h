#pragma once

#include "ui_proxy_settings_widget.h"
#include "settings_page.h"

namespace SeoSpider
{

class ProxySettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	ProxySettingsWidget(QWidget* parent = nullptr);

protected:
	virtual void init() override;

private slots:
	void onCheckBoxToggled();

private:
	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::ProxySettingsWidget m_ui;
};

}