#pragma once

#include "ui_user_agent_settings_widget.h"
#include "settings_page.h"

namespace SeoSpider
{

class UserAgentSettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	UserAgentSettingsWidget(QWidget* parent = nullptr);
	virtual void applyChanges() noexcept override;

	Q_SLOT void checkBoxStateChanged(bool state);

private:
	void initialize();
	void loadUserAgentsFromFile(const QString& fileName, QComboBox* targetComboBox);

private:
	Ui::UserAgentSettingsWidget m_ui;
};

}