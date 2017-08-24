#pragma once

#include "proxy_settings_dialog.h"
#include "limits_settings_dialog.h"
#include "application_settings_widget.h"

namespace QuickieWebBot
{

class MainFrameController;

#ifdef QT_DEBUG
class DebugInfoWebPageWidget;
#endif // DEBUG


class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget* parent = nullptr);

	//
	// settings pages
	//
	Q_SLOT void showProxySettingsDialog();
	Q_SLOT void showLimitsSettingsDialog();
	Q_SLOT void showApplicationSettingsWidget();

	Q_SIGNAL void onAboutMainFrameToClose();

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	void init();
	void createActions();
	void createAndSetCentralWidget();

private:
	MainFrameController* m_mainFrameController;

	ProxySettingsDialog* m_proxySettingsDialog;
	LimitsSettingsDialog* m_limitsSettingsDialog;
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