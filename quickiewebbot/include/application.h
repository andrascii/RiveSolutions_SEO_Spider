#pragma once

#include "main_frame.h"
#include "software_branding.h"

namespace QuickieWebBot
{

class ApplicationProperties;
class ModelController;
class WebCrawler;

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);

	const SoftwareBranding* softwareBrandingOptions() const noexcept;
	const ApplicationProperties* properties() const noexcept;
	const WebCrawler* webCrawler() const noexcept;
	ModelController* modelController() noexcept;
	MainFrame* mainFrame() noexcept;

	void initializeStyleSheet() noexcept;

private:
	Q_SLOT void mainFrameIsReadyForShow();

private:
	void initialize() noexcept;
	void showStartScreen() const noexcept;

private:
	ApplicationProperties* m_appicationProperties;
	ModelController* m_modelController;
	WebCrawler* m_webCrawler;
	
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;
};

}
