#pragma once

namespace QuickieWebBot
{

class ApplicationSettingsWidget;
class SettingsPageRegistry;

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget* parent = nullptr);

	Q_SLOT void showApplicationSettingsWidget();

private:
	void init();
	void createActions();
	void createAndSetCentralWidget();
	void registerSettingsPages() const;

	Q_SLOT void startCrawler();
	Q_SLOT void stopCrawler();
	Q_SLOT void clearCrawledData();

private:
	ApplicationSettingsWidget* m_applicationSettingsWidget;
};

}