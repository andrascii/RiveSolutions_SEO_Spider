#include "main_frame.h"
#include "model_controller.h"
#include "model_data_accessor_stub.h"
#include "model_data_accessor.h"
#include "data_collection.h"
#include "table_view_span_extension.h"
#include "application.h"
#include "web_crawler.h"
#include "hyperlink_item_delegate.h"

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
	VERIFY(connect(ui.startOrConrinueCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::start));
	VERIFY(connect(ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::stop));

	ui.viewTypeComboBox->addItem(tr("List"));
	ui.viewTypeComboBox->addItem(tr("Tree"));

	//////////////////////////////////////////////////////////////////////////
	// Debug code

	TableModel* model = new TableModel(this);
	//static ModelDataAccessorStub s_stub;
	//model->setDataAccessor(s_stub.allProcessedItems());

	
	model->setDataAccessor(std::make_unique<ModelDataAccessorAllItems>(DataCollection::CrawledUrlStorageType));

	ui.crawlingTableView->setModel(model);
	ui.crawlingTableView->setItemDelegate(new HyperlinkItemDelegate(ui.crawlingTableView));
	new TableViewSpanExtension(ui.crawlingTableView);
	//////////////////////////////////////////////////////////////////////////
}

}