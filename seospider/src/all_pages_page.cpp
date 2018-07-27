#include "all_pages_page.h"
#include "application.h"
#include "table_view.h"
#include "storage_adapter_factory.h"
#include "page_model.h"
#include "page_view_model.h"
#include "command_menu.h"
#include "page_data_widget.h"
#include "model_helpers.h"
#include "page_data_widget_splitter.h"

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

	m_pageDataWidget = new PageDataWidget(this);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksOnThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksToThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::ServerResponseForPageType);

	m_splitter = new PageDataWidgetSplitter(this, crawlingTableView, m_pageDataWidget);
	layout->addWidget(m_splitter);

	VERIFY(connect(crawlingTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		m_pageDataWidget, SLOT(pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(crawlingTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&))));
}

void AllPagesPage::pageViewSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(selected);
	Q_UNUSED(deselected);

	QItemSelectionModel* itemSelectionModel = qobject_cast<QItemSelectionModel*>(sender());
	DEBUG_ASSERT(itemSelectionModel);

	if (itemSelectionModel->selectedRows().isEmpty())
	{
		return;
	}

	QModelIndex index = itemSelectionModel->selectedRows().last();
	const PageModel* storageModel = getUnderlyingModelByIndex<PageModel*>(index);;

	if (storageModel)
	{
		const IStorageAdapter* storageAdapter = storageModel->storageAdapter();
		m_pageDataWidget->setParsedPageInfo(storageAdapter->parsedPageInfoPtr(index));
	}
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
