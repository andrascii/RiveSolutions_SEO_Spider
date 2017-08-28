#pragma once

#include "ui_application_settings_widget.h"

namespace QuickieWebBot
{

class ApplicationSettingsWidget : public QWidget
{
	Q_OBJECT

public:
	ApplicationSettingsWidget(QWidget* parent = nullptr);
	~ApplicationSettingsWidget();

private:
	Q_SLOT void applyChanges();

private:
	void initialize();

	Ui::ApplicationSettingsWidget m_ui;

	QByteArray m_currentPage;
	QStackedWidget* m_stackedWidget;
};

}