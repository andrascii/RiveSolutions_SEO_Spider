#pragma once

#include "application_settings_widget.h"

namespace QuickieWebBot
{

class MainFrameController;

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
	void createSettingsPages();

private:
	MainFrameController* m_mainFrameController;

	ApplicationSettingsWidget* m_applicationSettingsWidget;
	
	QAction* m_openFileAction;
	QAction* m_closeFileAction;
	QAction* m_recentFilesAction;
	QAction* m_saveFileAction;
	QAction* m_saveFileAsAction;
	QAction* m_exitProgramAction;

	QAction* m_openSettingsWidget;

};

}