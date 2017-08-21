#pragma once

#include "proxy_settings_dialog.h"
#include "limits_settings_dialog.h"

namespace QuickieWebBot
{

class MainFrameController;
class DebugInfoWebPageWidget;

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

private:
	MainFrameController* m_mainFrameController;

	ProxySettingsDialog* m_proxySettingsDialog;
	LimitsSettingsDialog* m_limitsSettingsDialog;

	std::unique_ptr<DebugInfoWebPageWidget> m_debugWebPageInfoWidget;
};

}