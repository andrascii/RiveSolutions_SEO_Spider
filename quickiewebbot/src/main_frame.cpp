#include "main_frame.h"
#include "model_controller.h"
#include "model_data_accessor_stub.h"
#include "model_data_accessor.h"
#include "model_controller_data.h"
#include "table_view_span_extension.h"
#include "application.h"
#include "crawler.h"
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

	VERIFY(connect(ui.actionAbout, &QAction::triggered, myApp, &Application::aboutQt));
	VERIFY(connect(ui.startOrConrinueCrawlingButton, &QPushButton::clicked, myApp->crawler(), &Crawler::start));
	VERIFY(connect(ui.stopCrawlingButton, &QPushButton::clicked, myApp->crawler(), &Crawler::stop));

	ui.viewTypeComboBox->addItem(tr("List"));
	ui.viewTypeComboBox->addItem(tr("Tree"));

	//////////////////////////////////////////////////////////////////////////
	// Debug code

	TableModel* model = new TableModel(this);
	//static ModelDataAccessorStub s_stub;
	//model->setDataAccessor(s_stub.allProcessedItems());

	
	model->setDataAccessor(std::make_unique<ModelDataAccessorAllItems>(ModelControllerData::CrawledUrlStorageType));

	ui.crawlingTableView->setModel(model);
	ui.crawlingTableView->setItemDelegate(new HyperlinkItemDelegate());
	new TableViewSpanExtension(ui.crawlingTableView);
	//////////////////////////////////////////////////////////////////////////
}

}