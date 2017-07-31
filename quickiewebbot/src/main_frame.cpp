#include "application.h"
#include "main_frame.h"
#include "model_controller.h"
#include "grid_data_accessor_factory.h"
#include "data_collection.h"
#include "web_crawler.h"
#include "grid_view_full_size_resize_strategy.h"
#include "quickie_web_bot_helpers.h"
#include "naviagation_panel_controller.h"
#include "context_menu_data_collection_row.h"
#include "igrid_model.h"
#include "summary_model.h"
#include "page_info_storage_model.h"


namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_proxySettingsDialog(nullptr)
	, m_limitsSettingsDialog(nullptr)
{
	initialize();
}

void MainFrame::showListView()
{
	m_ui.mainGuiStackedWidget->setCurrentIndex(0);
}

void MainFrame::showSummaryView()
{
	m_ui.mainGuiStackedWidget->setCurrentIndex(1);
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

void MainFrame::initialize()
{
	m_ui.setupUi(this);

 	VERIFY(connect(m_ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
	VERIFY(connect(m_ui.actionProxy, &QAction::triggered, this, &MainFrame::showProxySettingsDialog));
	VERIFY(connect(m_ui.actionSpiderSettings, &QAction::triggered, this, &MainFrame::showLimitsSettingsDialog));

	IGridModel* model = new PageInfoStorageModel(this);

	m_ui.crawlingGridView->setModel(model);
	m_ui.crawlingGridView->setContextMenu(new ContextMenuDataCollectionRow(m_ui.crawlingGridView));

	SummaryModel* summaryModel = new SummaryModel(this);
	m_ui.summaryGridView->setModel(summaryModel);

	m_ui.summaryGridView->setMinimumWidth(QuickieWebBotHelpers::pointsToPixels(350));

	new NavigationPanelController(this, &m_ui);
	
}

}