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

protected:
	virtual void init() override;

private slots:
	void checkBoxStateChanged(bool state);
	void groupButtonStateChanged();

private:
	void loadUserAgentsFromFile(const QString& fileName, QComboBox* targetComboBox);

	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::UserAgentSettingsWidget m_ui;
};

}