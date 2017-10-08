#pragma once

#include "message_box_dialog.h"

namespace QuickieWebBot
{

class ApplicationSettingsWidget;

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget* parent = nullptr);

signals:
	void crawlerStarted();
	void crawlerStopped();

public slots:
	void showApplicationSettingsWidget();
	void showMessageBoxDialog(const QString& title, const QString& message, MessageBoxDialog::Icon icon);

private:
	void init();
	void createActions();
	void createAndSetCentralWidget();
	void registerSettingsPages() const;

private slots:
	void startCrawler();
	void stopCrawler();
	void clearCrawledData();

private:
	ApplicationSettingsWidget* m_applicationSettingsWidget;
};

}