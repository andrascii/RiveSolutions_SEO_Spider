#pragma once

#include "main_window.h"
#include "software_branding.h"
#include "summary_data_accessor_factory.h"
#include "storage_adaptor_factory.h"
#include "isettings_accessor.h"
#include "requester.h"
#include "iresponse.h"
#include "requester_wrapper.h"

namespace WebCrawler
{

class SequencedDataCollection;
class Crawler;

}

namespace QuickieWebBot
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
	
	WebCrawler::Crawler* crawler() const noexcept;
	WebCrawler::SequencedDataCollection* sequencedDataCollection() const noexcept;

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

	void initializeStyleSheet() noexcept;

	// ISettingsAccessor implementation
	virtual void saveToSettings(const QByteArray& key, const QVariant& value) noexcept override;
	virtual QVariant loadFromSettings(const QByteArray& key, const QVariant& defaultValue = QVariant()) const noexcept override;
	virtual void removeKeyFromSettings(const QByteArray& key) override;
	virtual QList<QByteArray> allKeys() const override;

private:
	Q_SLOT void showMainWindow();

	static void registerServices();
	static QString operatingSystemVersion();

	void initQSettings();
	QSettings* settings() const;

private:
	void initialize() noexcept;

private:
	Preferences* m_preferences;
	WebCrawler::Crawler* m_crawler;

	WebCrawler::SequencedDataCollection* m_sequencedDataCollection;
	
	std::unique_ptr<MainWindow> m_mainWindow;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;

	std::unique_ptr<StorageAdaptorFactory> m_storageAdatpterFactory;
	std::unique_ptr<SummaryDataAccessorFactory> m_summaryDataAccessorFactory;

	QSettings* m_settings;
	QTranslator* m_translator;
};

}
