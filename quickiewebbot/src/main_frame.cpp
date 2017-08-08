#include "application.h"
#include "main_frame.h"
#include "model_controller.h"
#include "data_collection.h"
#include "web_crawler.h"
#include "grid_view_full_size_resize_policy.h"
#include "quickie_web_bot_helpers.h"
#include "naviagation_panel_controller.h"
#include "context_menu_data_collection_row.h"
#include "igrid_model.h"
#include "summary_model.h"
#include "page_info_storage_model.h"
#include "storage_adaptor.h"
#include "page_info_storage_view_model.h"
#include "summary_view_model.h"
#include "storage_adaptor_factory.h"
#include "main_frame_controller.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_mainFrameController(new MainFrameController(this, this))
	, m_proxySettingsDialog(nullptr)
	, m_limitsSettingsDialog(nullptr)
{
	init();

	//installEventFilter()
}

// void MainFrame::showListView()
// {
// 	m_ui.mainGuiStackedWidget->setCurrentIndex(0);
// }
// 
// void MainFrame::showSummaryView()
// {
// 	m_ui.mainGuiStackedWidget->setCurrentIndex(1);
// }

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

void MainFrame::init()
{
	setCentralWidget(new NavigationPanelWidget(this));
// 
//  	VERIFY(connect(m_ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
// 	VERIFY(connect(m_ui.actionProxy, &QAction::triggered, this, &MainFrame::showProxySettingsDialog));
// 	VERIFY(connect(m_ui.actionSpiderSettings, &QAction::triggered, this, &MainFrame::showLimitsSettingsDialog));
	
}

}