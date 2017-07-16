#include "main_frame.h"
#include "model_controller.h"
#include "model_data_accessor_stub.h"
#include "model_data_accessor_factory.h"
#include "data_collection.h"
#include "application.h"
#include "grid_view_extension.h"
#include "grid_view_delegate.h"
#include "web_crawler.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
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

void MainFrame::showProxySettingsWidget()
{
	if (!m_proxySettingsWidget)
	{
		m_proxySettingsWidget = std::make_unique<ProxySettingsWidget>();
	}

	m_proxySettingsWidget->show();
}

void MainFrame::initialize()
{
	m_ui.setupUi(this);

 	VERIFY(connect(m_ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
	VERIFY(connect(m_ui.actionProxy, &QAction::triggered, this, &MainFrame::showProxySettingsWidget));

	GridViewModel* model = new GridViewModel(this);

	ModelDataAccessorFactory factory;
	model->setDataAccessor(factory.getModelDataAccessor(ModelDataAccessorFactoryParams()));
	m_ui.crawlingTableView->setModel(model);

	m_ui.crawlingTableView->setItemDelegate(new GridViewDelegate(m_ui.crawlingTableView));
	new GridViewExtension(m_ui.crawlingTableView);
}

}