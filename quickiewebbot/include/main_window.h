#pragma once

#include "message_box_dialog.h"

namespace QuickieWebBot
{

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

private slots:
	void startCrawler();
	void stopCrawler();
	void clearCrawledData();

private:
	ApplicationSettingsWidget* m_applicationSettingsWidget;

	bool m_initialized;
};

}