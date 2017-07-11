#include "main_frame.h"
#include "model_controller.h"
#include "model_data_accessor_stub.h"
#include "model_data_accessor_factory.h"
#include "data_collection.h"
#include "application.h"
#include "crawler.h"
#include "gridview_extension.h"
#include "gridview_delegate.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
{
	init();
}

void MainFrame::init()
{
	ui.setupUi(this);

	VERIFY(connect(ui.actionAbout, &QAction::triggered, theApp, &Application::aboutQt));
	VERIFY(connect(ui.startOrConrinueCrawlingButton, &QPushButton::clicked, theApp->crawler(), &Crawler::start));
	VERIFY(connect(ui.stopCrawlingButton, &QPushButton::clicked, theApp->crawler(), &Crawler::stop));

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