#include "main_frame.h"
#include "model_controller.h"
#include "model_data_accessor_stub.h"
#include "model_data_accessor.h"
#include "model_controller_data.h"
#include "table_view_span_extension.h"

namespace QuickieWebBot
{

MainFrame::MainFrame(ModelControllerData* data, QWidget* parent)
	: QMainWindow(parent)
	, m_data(data)
{
	init();
}

void MainFrame::init()
{
	ui.setupUi(this);

	VERIFY(connect(ui.actionAbout, &QAction::triggered, qApp, &QApplication::aboutQt));

	ui.viewTypeComboBox->addItem(tr("List"));
	ui.viewTypeComboBox->addItem(tr("Tree"));

	//////////////////////////////////////////////////////////////////////////
	// Debug code

	TableModel* model = new TableModel(this);
	static ModelDataAccessorStub s_stub;
	model->setDataAccessor(s_stub.allProcessedItems());
	
	//model->setDataAccessor(std::make_unique<ModelDataAccessorAllItems>(m_data, ModelControllerData::CrawledUrlStorageType));

	ui.crawlingTableView->setModel(model);
	new TableViewSpanExtension(ui.crawlingTableView);
	//////////////////////////////////////////////////////////////////////////
}

}