#pragma once

#include "application_settings_model.h"

#include "ui_application_settings_widget.h"
#include "crawler_settings_widget.h"

namespace QuickieWebBot
{

class ApplicationSettingsWidget : public QWidget
{
	Q_OBJECT

public:
	ApplicationSettingsWidget(QWidget* parent = nullptr);

private:
	void initialize();

	Ui::ApplicationSettingsWidget m_ui;
	//CrawlerSettingsWidget m_crawlerSettingsWidget;
};

}