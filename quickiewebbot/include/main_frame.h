#pragma once

#include "application_settings_widget.h"

namespace QuickieWebBot
{

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget* parent = nullptr);

	//
	// settings pages
	//
	Q_SLOT void showApplicationSettingsWidget();

	Q_SIGNAL void onAboutMainFrameToClose();

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	void init();
	void createActions();
	void createAndSetCentralWidget();

	Q_SLOT void startCrawler();
	Q_SLOT void stopCrawler();
	Q_SLOT void clearCrawledData();

private:
	ApplicationSettingsWidget* m_applicationSettingsWidget;
};

}