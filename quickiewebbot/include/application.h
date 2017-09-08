#pragma once

#include "main_frame.h"
#include "software_branding.h"
#include "summary_data_accessor_factory.h"
#include "storage_adaptor_factory.h"

namespace WebCrawler
{

class WebCrawler;
class GuiStorage;

}

namespace QuickieWebBot
{

class ApplicationProperties;

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);
	~Application();

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

	ApplicationProperties* properties() noexcept;
	MainFrame* mainFrame() noexcept;
	StorageAdaptorFactory* storageAdaptorFactory() noexcept;
	SummaryDataAccessorFactory* summaryDataAccessorFactory() noexcept;

	WebCrawler::WebCrawler* webCrawler() noexcept;
	WebCrawler::GuiStorage* guiStorage() noexcept;

	void initializeStyleSheet() noexcept;

	QString operatingSystemVersion() const noexcept;

private:
	Q_SLOT void mainFrameIsReadyForShow();

	void registerSettingsPages() const;
	void registerServices() const;

private:
	void initialize() noexcept;
	void showSplashScreen() const noexcept;

private:
	ApplicationProperties* m_appicationProperties;
	std::unique_ptr<WebCrawler::WebCrawler> m_webCrawler;

	WebCrawler::GuiStorage* m_guiStorage;
	
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;

	std::unique_ptr<StorageAdaptorFactory> m_storageAdatpterFactory;
	std::unique_ptr<SummaryDataAccessorFactory> m_summaryDataAccessorFactory;
};

}
