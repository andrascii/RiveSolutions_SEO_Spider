#pragma once

#include "main_window.h"
#include "software_branding.h"
#include "summary_data_accessor_factory.h"
#include "storage_adaptor_factory.h"
#include "isettings_accessor.h"
#include "requester.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

struct GetHostInfoResponse;
class SequencedDataCollection;
class Crawler;

}

namespace SeoSpider
{

class Preferences;

class Application : public QApplication, public ISettingsAccessor
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);
	~Application();

	Preferences* preferences() const noexcept;
	MainWindow* mainWindow() const noexcept;
	StorageAdaptorFactory* storageAdaptorFactory() const noexcept;
	SummaryDataAccessorFactory* summaryDataAccessorFactory() const noexcept;

	CrawlerEngine::Crawler* crawler() const noexcept;
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection() const noexcept;

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

	void initializeStyleSheet() noexcept;

	// ISettingsAccessor implementation
	virtual void saveToSettings(const QByteArray& key, const QVariant& value) noexcept override;
	virtual QVariant loadFromSettings(const QByteArray& key, const QVariant& defaultValue = QVariant()) const noexcept override;
	virtual void removeKeyFromSettings(const QByteArray& key) override;
	virtual QList<QByteArray> allKeys() const override;

private:
	static void registerServices();
	static QString operatingSystemVersion();

	void initQSettings();
	QSettings* settings() const;

	void onHostInfoResponse(Common::Requester* requester, const CrawlerEngine::GetHostInfoResponse& response);

	private slots:
	void startCrawler();
	void stopCrawler();
	void clearCrawledData();
	void showMainWindow();

private:
	void initialize() noexcept;

private:
	Preferences* m_preferences;
	std::unique_ptr<CrawlerEngine::Crawler> m_crawler;

	CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;

	std::unique_ptr<MainWindow> m_mainWindow;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;

	std::unique_ptr<StorageAdaptorFactory> m_storageAdatpterFactory;
	std::unique_ptr<SummaryDataAccessorFactory> m_summaryDataAccessorFactory;

	QSettings* m_settings;
	QTranslator* m_translator;

	Common::RequesterWrapper m_hostInfoRequester;
};

}