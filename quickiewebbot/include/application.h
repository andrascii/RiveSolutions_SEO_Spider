#pragma once

#include "main_frame.h"
#include "downloader.h"
#include "software_branding.h"

namespace QuickieWebBot
{

class ModelController;
class Crawler;

class Application : public QApplication
{
	Q_OBJECT

public:
	static Application* instance();
	Application(int& argc, char** argv);

	const Crawler* crawler() const noexcept;
	MainFrame* mainFrame() noexcept;
	ModelController* modelController() noexcept;

	const SoftwareBranding* softwareBrandingOptions() const noexcept;

private:
	Q_SLOT void mainFrameReadyForShow();

private:
	void initialize() noexcept;
	void initializeStyleSheet() noexcept;
	void showStartScreen() const noexcept;

private:
	static Application* s_app;
	
	ModelController* m_modelController;
	
	Crawler* m_crawler;
	
	std::unique_ptr<Downloader> m_downloader;
	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;
};

}
