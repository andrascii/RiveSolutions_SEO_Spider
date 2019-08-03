#pragma once
#include "stdafx.h"

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

protected:
	virtual void init() override;

private:
	void loadLanguagesFromFile(const QString& fileName);

	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::LanguageSettingsWidget m_ui;

};

}