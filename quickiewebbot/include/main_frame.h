#pragma once

#include "proxy_settings_dialog.h"
#include "limits_settings_dialog.h"

namespace QuickieWebBot
{

class MainFrameController;

#ifdef DEBUG
class DebugInfoWebPageWidget;
#endif // DEBUG


class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget* parent = nullptr);
	~MainFrame();

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

#ifdef DEBUG
	std::unique_ptr<DebugInfoWebPageWidget> m_debugWebPageInfoWidget;
#endif // DEBUG
};

}