#pragma once

#include "ui_main_frame.h"
#include "proxy_settings_widget.h"

namespace QuickieWebBot
{

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);

	void showListView();
	void showSummaryView();

	//
	// settings pages
	//
	Q_SLOT void showProxySettingsWidget();

private:
	void initialize();

private:
	Ui::MainFrameClass m_ui;

	std::unique_ptr<ProxySettingsWidget> m_proxySettingsWidget;
};

}