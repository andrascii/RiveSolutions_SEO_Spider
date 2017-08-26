#pragma once

#include "main_frame.h"
#include "software_branding.h"
#include "summary_data_accessor_factory.h"
#include "storage_adaptor_factory.h"

namespace WebCrawler
{

class WebCrawler;
class ModelController;

}

namespace QuickieWebBot
{

class ApplicationProperties;

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

	WebCrawler::WebCrawler* webCrawler() noexcept;
	ApplicationProperties* properties() noexcept;

	WebCrawler::ModelController* modelController() noexcept;

	StorageAdaptorFactory* storageAdaptorFactory() noexcept;
	SummaryDataAccessorFactory* summaryDataAccessorFactory() noexcept;

	MainFrame* mainFrame() noexcept;

	void initializeStyleSheet() noexcept;

	QString operatingSystemVersion() const noexcept;

private:
	Q_SLOT void mainFrameIsReadyForShow();

private:
	void initialize() noexcept;
	void showSplashScreen() const noexcept;

private:
	ApplicationProperties* m_appicationProperties;
	WebCrawler::ModelController* m_modelController;
	WebCrawler::WebCrawler* m_webCrawler;
	
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;

	std::unique_ptr<StorageAdaptorFactory> m_storageAdatpterFactory;
	std::unique_ptr<SummaryDataAccessorFactory> m_summaryDataAccessorFactory;
};

}
