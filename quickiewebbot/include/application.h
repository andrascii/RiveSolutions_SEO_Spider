#pragma once

#include "main_frame.h"
#include "software_branding.h"
#include "summary_data_accessor_factory.h"
#include "storage_adaptor_factory.h"
#include "isettings_accessor.h"

namespace WebCrawler
{

class WebCrawler;
class GuiStorage;

}

namespace QuickieWebBot
{

class ApplicationSettings;

class Application : public QApplication, public ISettingsAccessor
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);
	~Application();

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

	ApplicationSettings* properties() noexcept;
	MainFrame* mainFrame() noexcept;
	StorageAdaptorFactory* storageAdaptorFactory() noexcept;
	SummaryDataAccessorFactory* summaryDataAccessorFactory() noexcept;
	
	WebCrawler::WebCrawler* webCrawler() noexcept;
	WebCrawler::GuiStorage* guiStorage() noexcept;

	void initializeStyleSheet() noexcept;

	QString operatingSystemVersion() const noexcept;

	virtual void saveToSettings(const QByteArray& key, const QVariant& value) noexcept override;
	virtual QVariant loadFromSettings(const QByteArray& key, const QVariant& defaultValue = QVariant()) const noexcept override;
	virtual void removeKeyFromSettings(const QByteArray& key) override;
	virtual QList<QByteArray> allKeys() const override;

private:
	Q_SLOT void mainFrameIsReadyForShow();

	void registerSettingsPages() const;
	void registerServices() const;

private:
	void initialize() noexcept;
	void showSplashScreen() const noexcept;

private:
	ApplicationSettings* m_appicationSettings;
	std::unique_ptr<WebCrawler::WebCrawler> m_webCrawler;

	WebCrawler::GuiStorage* m_guiStorage;
	
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;

	std::unique_ptr<StorageAdaptorFactory> m_storageAdatpterFactory;
	std::unique_ptr<SummaryDataAccessorFactory> m_summaryDataAccessorFactory;
};

}
