#pragma once

#include "settings_page.h"
#include "ui_crawler_settings_widget.h"

namespace SeoSpider
{

class CrawlerSettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	CrawlerSettingsWidget(QWidget* parent = nullptr);

protected:
	virtual void init() override;

private:
	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::CrawlerSettingsWidget m_ui;
};

}