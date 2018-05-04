#pragma once

#include "ui_language_settings_widget.h"
#include "settings_page.h"

namespace SeoSpider
{
	
class LanguageSettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	LanguageSettingsWidget(QWidget* parent = nullptr);
	virtual void applyChanges() noexcept override;

private:
	virtual void init() override;
	void loadLanguagesFromFile(const QString& fileName, QComboBox* targetComboBox);

private:
	Ui::LanguageSettingsWidget m_ui;

};

}