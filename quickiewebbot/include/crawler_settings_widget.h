#pragma once

#include "ui_crawler_settings_widget.h"

namespace QuickieWebBot
{

class CrawlerSettingsWidget : public QFrame
{
	Q_OBJECT

public:
	CrawlerSettingsWidget(QWidget* parent = nullptr);

private:
	void initialize();

	Ui::CrawlerSettingsWidget m_ui;
};

}