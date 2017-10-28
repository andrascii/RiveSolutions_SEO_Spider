#pragma once

#include "message_box_dialog.h"
#include "requester_wrapper.h"

namespace Common
{

class Requester;

}

namespace QuickieWebBot
{

struct GetHostInfoResponse;
class ApplicationSettingsWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void init();

signals:
	void crawlerStarted();
	void crawlerStopped();

public slots:
	void showApplicationSettingsWidget();
	void showMessageBoxDialog(const QString& title, const QString& message, MessageBoxDialog::Icon icon);

private:
	void createActions() const;
	void createAndSetCentralWidget();
	void registerSettingsPages() const;

	void onHostInfoResponse(Common::Requester* requester, const GetHostInfoResponse& response);

private slots:
	void startCrawler();
	void stopCrawler();
	void clearCrawledData();

private:
	ApplicationSettingsWidget* m_applicationSettingsWidget;

	bool m_initialized;

	Common::RequesterWrapper m_requester;
};

}