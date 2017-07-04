#pragma once

#include "main_frame.h"
#include "software_branding.h"

namespace QuickieWebBot
{

class ModelController;

class Application : public QApplication
{
	Q_OBJECT

public:
	static Application* instance();
	Application(int& argc, char** argv);

	MainFrame* mainFrame() const noexcept;
	SoftwareBranding const* softwareBrandingOptions() const noexcept;

private:
	Q_SLOT void mainFrameReadyForShow();

private:
	void initialize() noexcept;
	void initializeStyleSheet() noexcept;
	void showStartScreen() const noexcept;

private:
	static Application* s_app;

	ModelController* m_modelController;

	std::unique_ptr<MainFrame> m_mainFrame;
	std::unique_ptr<SoftwareBranding> m_softwareBrandingOptions;
};

}
