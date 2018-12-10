#pragma once

#include "requester_wrapper.h"

namespace SeoSpider
{

class ContentFrame;
class IUpdateChecker;
class ShadedOverlay;
class ApplicationSettingsDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void init();
	ContentFrame* contentFrame() const noexcept;
	QSystemTrayIcon* systemTrayIcon() const noexcept;

signals:
	void resized();
	void moved();
	void windowStateChanged();

public slots:
	void showSitemapCreatorDialog();
	void saveFile();
	void saveFileAs();
	void openFile();
	void closeFile();
	void openRecentFile();
	void saveFileAndClearData();
	void showApplicationSettingsDialog(const QByteArray& settingsPageName = QByteArray());
	void onCrawlingFinished() const;
	void showShadedOverlay();
	void hideShadedOverlay();
	void onCustomUrlReceived();

	int showMessageBoxDialog(const QString& title,
		const QString& message,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel) const;

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void moveEvent(QMoveEvent* event) override;
	virtual void changeEvent(QEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;

private:
	void openFile(const QString& filePath);
	void createActions();
	void createAndSetCentralWidget();
	void registerSettingsPages() const;
	void clearDataOnSerializationDone();
	void initSystemTrayIconMenu();
	void loadState();
	QString getSaveFilePath() const;

	void initHelpActions();
	void initFileActions();
	void initSettingsActions();
	void initCrawlerActions();

private slots:
	void onCrawlerSessionCreated();
	void onCrawlerSessionDestroyed();
	void onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void showRegisterProductDialog();
	void showFeedbackDialog();
	void onAboutUpdateExists();
	void onAboutUpdateIsNotExists();
	void openHelpPage();

private:
	bool m_initialized;

	CrawlerEngine::RequesterWrapper m_requester;
	ContentFrame* m_contentFrame;
	QSystemTrayIcon* m_systemTrayIcon;

	IUpdateChecker* m_updateChecker;
	ShadedOverlay* m_shadedOverlay;
	ApplicationSettingsDialog* m_applicationSettingsDialog;
};

}
