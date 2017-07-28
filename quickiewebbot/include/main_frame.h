#pragma once

#include "ui_main_frame.h"
#include "proxy_settings_dialog.h"
#include "limits_settings_dialog.h"

namespace QuickieWebBot
{

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = nullptr);

	void showListView();
	void showSummaryView();

	//
	// settings pages
	//
	Q_SLOT void showProxySettingsDialog();
	Q_SLOT void showLimitsSettingsDialog();

private:
	void initialize();

private:
	Ui::MainFrameClass m_ui;

	ProxySettingsDialog* m_proxySettingsDialog;
	LimitsSettingsDialog* m_limitsSettingsDialog;
};

}