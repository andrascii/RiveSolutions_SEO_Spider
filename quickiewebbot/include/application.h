#pragma once

#include "main_frame.h"
#include "software_branding.h"

namespace QuickieWebBot
{

class ModelController;
class WebCrawler;

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int& argc, char** argv);

	const WebCrawler* webCrawler() const noexcept;
	MainFrame* mainFrame() noexcept;
	ModelController* modelController() noexcept;

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

private:
	Q_SLOT void mainFrameIsReadyForShow();

private:
	void initialize() noexcept;
	void initializeStyleSheet() noexcept;
	void showStartScreen() const noexcept;

private:
	ModelController* m_modelController;
	
	WebCrawler* m_webCrawler;
	
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;
};

}
