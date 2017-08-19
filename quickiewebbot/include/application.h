#pragma once

#include "main_frame.h"
#include "software_branding.h"

namespace WebCrawler
{

class WebCrawler;
class ModelController;

}

namespace QuickieWebBot
{

class ApplicationProperties;
class StorageAdaptorFactory;


class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);
	~Application();

	const SoftwareBranding* softwareBrandingOptions() const noexcept;
	WebCrawler::WebCrawler* webCrawler() noexcept;
	ApplicationProperties* properties() noexcept;
	WebCrawler::ModelController* modelController() noexcept;
	StorageAdaptorFactory* storageAdapterFactory() noexcept;
	MainFrame* mainFrame() noexcept;

	void initializeStyleSheet() noexcept;

	QString operatingSystemVersion() const noexcept;

private:
	Q_SLOT void mainFrameIsReadyForShow();

private:
	void initialize() noexcept;
	void showStartScreen() const noexcept;

private:
	ApplicationProperties* m_appicationProperties;
	WebCrawler::ModelController* m_modelController;
	WebCrawler::WebCrawler* m_webCrawler;

	std::unique_ptr<StorageAdaptorFactory> m_storageAdatpterFactory;
	
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;
};

}
