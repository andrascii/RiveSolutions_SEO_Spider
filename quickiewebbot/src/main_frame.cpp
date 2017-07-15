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

	VERIFY(connect(ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
	VERIFY(connect(ui.startOrConrinueCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::startCrawling));
	VERIFY(connect(ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::stopCrawling));

	using pushButtonToggledFunction = void (QAbstractButton::*)(bool);
	VERIFY(connect(ui.generalViewButton, static_cast<pushButtonToggledFunction>(&QPushButton::toggled), [&ui = ui](bool checked)
	{ 
		ui.summaryViewButton->setChecked(!checked);
		if (checked)
		{
			ui.mainGuiStackedWidget->setCurrentIndex(0);
		}
	}));

	VERIFY(connect(ui.summaryViewButton, static_cast<pushButtonToggledFunction>(&QPushButton::toggled), [&ui = ui](bool checked)
	{
		ui.generalViewButton->setChecked(!checked);
		if (checked)
		{
			ui.mainGuiStackedWidget->setCurrentIndex(1);
		}
	}));


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