#include "application.h"
#include "main_frame.h"
#include "model_controller.h"
#include "model_data_accessor_stub.h"
#include "model_data_accessor_factory.h"
#include "data_collection.h"
#include "grid_view_extension.h"
#include "web_crawler.h"
#include "grid_view_full_size_resize_strategy.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_proxySettingsDialog(nullptr)
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

void MainFrame::initialize()
{
	m_ui.setupUi(this);

 	VERIFY(connect(m_ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
	VERIFY(connect(m_ui.actionProxy, &QAction::triggered, this, &MainFrame::showProxySettingsDialog));

	GridViewModel* model = new GridViewModel(this);

	ModelDataAccessorFactory factory;
	model->setModelDataAccessor(factory.getModelDataAccessor(ModelDataAccessorFactoryParams{ ModelDataAccessorFactoryParams::TypeAllCrawledUrls }));
	m_ui.crawlingGridView->setModel(model);

	GridViewModel* summaryModel = new GridViewModel(this);
	summaryModel->setModelDataAccessor(factory.getModelDataAccessor(ModelDataAccessorFactoryParams{ ModelDataAccessorFactoryParams::TypeSummary }));

	m_ui.summaryGridView->setModel(summaryModel);
	//m_ui.summaryGridView->setColumnResizeStrategy(std::make_unique<GridViewFullSizeResizeStrategy>(std::vector<int>{ 60, 40 }));


	VERIFY(connect(m_ui.summaryGridView, SIGNAL(childViewParamsChanged(const ModelDataAccessorFactoryParams&)), 
		m_ui.summaryDetailsGridView, SLOT(setParams(const ModelDataAccessorFactoryParams&))));

	m_ui.summaryGridView->setMinimumWidth(QuickieWebBotHelpers::pointsToPixels(350));
}

}