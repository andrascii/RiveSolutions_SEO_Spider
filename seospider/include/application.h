#pragma once

#include "isettings_accessor.h"
#include "crawler_options.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class Crawler;
class SequencedDataCollection;

}

namespace SeoSpider
{

class Preferences;
class MainWindow;
class SoftwareBranding;
class StorageAdapterFactory;
class SummaryDataAccessorFactory;
class InternetConnectionNotificationManager;
class CommandLineHandler;
class ICrawlerOptions;

class Application : public QApplication, public ISettingsAccessor
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);
	~Application();

	Preferences* preferences() const noexcept;
	MainWindow* mainWindow() const noexcept;
	StorageAdapterFactory* storageAdapterFactory() const noexcept;
	SummaryDataAccessorFactory* summaryDataAccessorFactory() const noexcept;
	InternetConnectionNotificationManager* internetConnectionNotificationManager() const noexcept;
	CrawlerEngine::Crawler* crawler() const noexcept;
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection() const noexcept;
	const SoftwareBranding* softwareBrandingOptions() const noexcept;
	const QString getOrGenerateUserID();

	void initializeStyleSheet();
	bool isInternetAvailable() const noexcept;

	virtual void saveToSettings(const QByteArray& key, const QVariant& value) noexcept override;
	virtual QVariant loadFromSettings(const QByteArray& key, const QVariant& defaultValue = QVariant()) const noexcept override;
	virtual void removeKeyFromSettings(const QByteArray& key) override;
	virtual QStringList allKeys() const override;

signals:
	void mainWindowShown();

private:
	static void registerServices();

private slots:
	void startCrawler();
	void stopCrawler();
	void clearCrawledData();
	void showMainWindow();
	void onAboutCrawlerOptionsChanged();
	void onAboutUseCustomUserAgentChanged();
	void closeWaitOperationFrame();

private:
	void initialize();
	void applicationInitialized();
	void attachPreferencesToCrawlerOptions();
	void openFileThroughCmd(const QString& path);
	void initQSettings();
	QSettings* settings() const;

private:
	std::unique_ptr<CommandLineHandler> m_commandLineHandler;
	Preferences* m_preferences;
	CrawlerEngine::Crawler* m_crawler;
	CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;
	std::unique_ptr<MainWindow> m_mainWindow;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;
	std::unique_ptr<StorageAdapterFactory> m_storageAdatpterFactory;
	std::unique_ptr<SummaryDataAccessorFactory> m_summaryDataAccessorFactory;
	QSettings* m_settings;
	QTranslator* m_translator;
	InternetConnectionNotificationManager* m_internetNotificationManager;
};

}