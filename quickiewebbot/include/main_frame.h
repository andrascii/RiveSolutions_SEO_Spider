#pragma once

#include "proxy_settings_dialog.h"
#include "limits_settings_dialog.h"

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

#ifdef QT_DEBUG

	std::unique_ptr<DebugInfoWebPageWidget> m_debugWebPageInfoWidget;

#endif // DEBUG

	QAction* m_openFileAction;
	QAction* m_closeFileAction;
	QAction* m_recentFilesAction;
	QAction* m_saveFileAction;
	QAction* m_saveFileAsAction;
	QAction* m_exitProgramAction;
};

}