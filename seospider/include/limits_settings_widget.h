#pragma once

#include "settings_page.h"
#include "ui_limits_settings_widget.h"

namespace SeoSpider
{

class LimitsSettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	LimitsSettingsWidget(QWidget* parent = nullptr);

protected:
	virtual void init() override;

private:
	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::LimitsSettingsWidget m_ui;
};

}