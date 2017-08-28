#pragma once

#include "ui_application_settings_widget.h"

namespace QuickieWebBot
{

class ApplicationSettingsWidget : public QDialog
{
	Q_OBJECT

public:
	ApplicationSettingsWidget(QWidget* parent = nullptr);
	~ApplicationSettingsWidget();

private:
	Q_SLOT void applyChanges();
	Q_SLOT void okButtonClicked();
	Q_SLOT void cancelButtonClicked();

private:
	void initialize();
	void reloadSettings();
	virtual void showEvent(QShowEvent* event);

private:
	Ui::ApplicationSettingsWidget m_ui;

	QByteArray m_currentPage;
	QStackedWidget* m_stackedWidget;

	bool m_somethingChanged;
};

}