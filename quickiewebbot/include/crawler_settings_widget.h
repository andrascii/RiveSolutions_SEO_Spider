#pragma once

#include "ui_crawler_settings_widget.h"
#include "isettings_page.h"

namespace QuickieWebBot
{

class CrawlerSettingsWidget : public QWidget, public ISettingsPage
{
	Q_OBJECT

public:
	CrawlerSettingsWidget(QWidget* parent = nullptr);

	virtual void applyChanges() noexcept override;
	virtual void reloadSettings() noexcept override;
	virtual bool isAutoApply() const noexcept override;

	Q_SLOT void somethingChangedSlot();

private:
	void initialize();

private:
	Ui::CrawlerSettingsWidget m_ui;
};

}