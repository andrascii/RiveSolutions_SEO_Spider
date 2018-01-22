#pragma once

#include "isettings_accessor.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

class SequencedDataCollection;
class Crawler;

}

namespace SeoSpider
{

class Preferences;
class MainWindow;
class SoftwareBranding;
class StorageAdapterFactory;
class SummaryDataAccessorFactory;
class HeaderControlsContainer;
class InternetConnectionNotificationManager;

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

	void initializeStyleSheet() noexcept;
	bool internetAvailable() const noexcept;

	// ISettingsAccessor implementation
	virtual void saveToSettings(const QByteArray& key, const QVariant& value) noexcept override;
	virtual QVariant loadFromSettings(const QByteArray& key, const QVariant& defaultValue = QVariant()) const noexcept override;
	virtual void removeKeyFromSettings(const QByteArray& key) override;
	virtual QStringList allKeys() const override;

	HeaderControlsContainer* headerControlsContainer() const;

signals:
	void mainWindowShown();

private:
	static void registerServices();
	static QString operatingSystemVersion();

	void initQSettings();
	QSettings* settings() const;

private slots:
	void startCrawler();
	void stopCrawler();
	void clearCrawledData();
	void showMainWindow();
	void onCrawlerOptionsChanged(CrawlerEngine::CrawlerOptions options);

private:
	void initialize();

private:
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
	std::unique_ptr<HeaderControlsContainer> m_headerControlsContainer;
};

}