#pragma once
#include "stdafx.h"

#include "ui_crawler_pause_settings_widget.h"
#include "settings_page.h"

namespace SeoSpider
{

class CrawlerPauseSettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	CrawlerPauseSettingsWidget(QWidget* parent = nullptr);

protected:
	virtual void init() override;

private slots:
	void fromCrawlerPauseSpinBoxChanged(int value);
	void toCrawlerPauseSpinBoxChanged(int value);
	void useCrawlerPauseCheckBoxChanged(int value);

private:
	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::CrawlerPauseSettingsWidget m_ui;
};

}