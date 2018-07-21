#include "all_pages_page.h"
#include "application.h"
#include "table_view.h"
#include "storage_adapter_factory.h"
#include "page_model.h"
#include "page_view_model.h"
#include "command_menu.h"

namespace SeoSpider
{

AllPagesPage::AllPagesPage(QWidget* parent)
	: QFrame(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	TableView* crawlingTableView = new TableView(this, false, true);
	PageModel* model = new PageModel;
	PageViewModel* modelView = new PageViewModel(crawlingTableView, model);

	IStorageAdapter* storageAdapter = theApp->storageAdapterFactory()->createParsedPageInfoStorage(
		StorageAdapterType::StorageAdapterTypeAllPages, theApp->sequencedDataCollection());

	model->setStorageAdapter(storageAdapter);

	crawlingTableView->setModel(model);
	crawlingTableView->setViewModel(modelView);
	crawlingTableView->setShowAdditionalGrid(true);
	crawlingTableView->setContextMenu(new CommandMenu(storageAdapter));

	layout->addWidget(crawlingTableView);
}

QWidget* AllPagesPage::widget() const
{
	return const_cast<AllPagesPage*>(this);
}

QString AllPagesPage::name() const
{
	return tr("All Site Pages");
}

QIcon AllPagesPage::icon() const
{
	QIcon icon;
	icon.addFile(QStringLiteral(":/images/all-pages-active.png"), QSize(), QIcon::Normal, QIcon::On);
	icon.addFile(QStringLiteral(":/images/all-pages-active.png"), QSize(), QIcon::Active, QIcon::On);
	icon.addFile(QStringLiteral(":/images/all-pages-active.png"), QSize(), QIcon::Active, QIcon::Off);
	icon.addFile(QStringLiteral(":/images/all-pages-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

	return icon;
}

IPage::Type AllPagesPage::type() const
{
	return IPage::AllPagesPage;
}

}