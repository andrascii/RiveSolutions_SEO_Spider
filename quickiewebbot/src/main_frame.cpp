#include "application.h"
#include "main_frame.h"
#include "model_controller.h"
#include "data_collection.h"
#include "web_crawler.h"
#include "quickie_web_bot_helpers.h"
#include "main_frame_controller.h"
#include "data_pages_widget.h"
#include "control_panel_widget.h"

#ifdef DEBUG
#include "debug_info_web_page_widget.h"
#endif // DEBUG



namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_mainFrameController(new MainFrameController(this, this))
	, m_proxySettingsDialog(nullptr)
	, m_limitsSettingsDialog(nullptr)
{
	init();
}

MainFrame::~MainFrame()
{

}

void MainFrame::showProxySettingsDialog()
{
	if (!m_proxySettingsDialog)
	{
		m_proxySettingsDialog = new ProxySettingsDialog(this);
	}

	m_proxySettingsDialog->exec();
}

void MainFrame::showLimitsSettingsDialog()
{
	if (!m_limitsSettingsDialog)
	{
		m_limitsSettingsDialog = new LimitsSettingsDialog(this);
	}

	m_limitsSettingsDialog->exec();
}

void MainFrame::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);

#ifdef DEBUG
	m_debugWebPageInfoWidget->close();
#endif // DEBUG

}

void MainFrame::init()
{
	QWidget* centralWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(centralWidget);

	layout->addWidget(new ControlPanelWidget(centralWidget));
	layout->addWidget(new DataPagesWidget(centralWidget));

	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);

#ifdef DEBUG
	m_debugWebPageInfoWidget.reset(new DebugInfoWebPageWidget(nullptr));
	m_debugWebPageInfoWidget->show();
#endif // DEBUG

}

}