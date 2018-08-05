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
	virtual bool eventFilter(QObject* object, QEvent* event) override;
	virtual void init() override;

private slots:
	void onCheckBoxToggled();

private:
	Ui::ProxySettingsWidget m_ui;
};

}