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

void MainFrame::initialize()
{
	ui.setupUi(this);

	auto showListView = [this](bool checked)
	{
		ui.summaryViewButton->setChecked(!checked);
		if (checked)
		{
			ui.mainGuiStackedWidget->setCurrentIndex(0);
		}
	};

	auto showSummaryView = [this](bool checked)
	{
		ui.generalViewButton->setChecked(!checked);
		if (checked)
		{
			ui.mainGuiStackedWidget->setCurrentIndex(1);
		}
	};

	VERIFY(connect(ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
	VERIFY(connect(ui.startOrConrinueCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::startCrawling));
	VERIFY(connect(ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::stopCrawling));

	VERIFY(connect(ui.generalViewButton, &QPushButton::toggled, showListView));
	VERIFY(connect(ui.summaryViewButton, &QPushButton::toggled, showSummaryView));


	ui.viewTypeComboBox->addItem(tr("List"));
	ui.viewTypeComboBox->addItem(tr("Tree"));

	//////////////////////////////////////////////////////////////////////////
	// Debug code

	GridViewModel* model = new GridViewModel(this);
	//static ModelDataAccessorStub s_stub;
	//model->setDataAccessor(s_stub.allProcessedItems());

	ModelDataAccessorFactory factory;
	model->setDataAccessor(factory.getModelDataAccessor(ModelDataAccessorFactoryParams()));
	ui.crawlingTableView->setModel(model);
	ui.crawlingTableView->setItemDelegate(new GridViewDelegate(ui.crawlingTableView));
	new GridViewExtension(ui.crawlingTableView);
	//////////////////////////////////////////////////////////////////////////
}

}