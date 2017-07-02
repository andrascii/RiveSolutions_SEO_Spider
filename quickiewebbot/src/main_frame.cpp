#include "main_frame.h"
#include "model_controller.h"

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

	VERIFY(connect(ui.actionAbout, &QAction::triggered, qApp, &QApplication::aboutQt));

	ui.viewTypeComboBox->addItem(tr("List"));
	ui.viewTypeComboBox->addItem(tr("Tree"));

	//////////////////////////////////////////////////////////////////////////
	// Debug code
	std::vector<WebsiteAnalyseElement>* dataStorage = new std::vector<WebsiteAnalyseElement>;

	for (int i = 0; i < 10; ++i)
	{
		dataStorage->push_back(
			WebsiteAnalyseElement
			{
				QUrl("http://test.com/index.php?lol=kek&cheburek=true"),
				"text/html; charset=utf8",
				"nothing",
				"nothing",
				"nothing",
				"200 OK",
				"LOL KEK CHEBUREK",
				"PAGE DESCRIBES LOL AND KEK AND CHEBUREK",
				"LOL, KEK, CHEBUREK, MEMOGRAMM, LOLOGRAMM, DNIWEGRAMM",
				"What's lol does means?",
				"What's kek does means?",
				"What is the cheburek?",
				"What is the cheburek?",
				"nothing",
				200,
				16,
				123,
				123,
				22,
				22,
				20,
				20,
				39691488,
				100500,
				100500
			});
	}

	TableModel* model = new TableModel(dataStorage, this);
	model->setHeaderItems(
		{
			"URL", 
			"content", 
			"meta refresh", 
			"meta robots", 
			"redirected url", 
			"server response", 
			"title", 
			"meta description", 
			"meta keywords", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			""}
	);

	ui.crawlingTableView->setModel(model);
	//////////////////////////////////////////////////////////////////////////
}

}