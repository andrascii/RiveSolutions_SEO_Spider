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
#include "lookup_lineedit_widget.h"

namespace SeoSpider
{

AllPagesPage::AllPagesPage(QWidget* parent)
	: QFrame(parent)
	, m_crawlingTableView(new TableView(this, false, true))
	, m_pageDataWidget(nullptr)
	, m_splitter(nullptr)
	, m_lookupLineEditWidget(nullptr)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	PageModel* model = new PageModel;
	PageViewModel* modelView = new PageViewModel(m_crawlingTableView, model);

	IStorageAdapter* storageAdapter = theApp->storageAdapterFactory()->createParsedPageInfoStorage(
		StorageAdapterType::StorageAdapterTypeAllPages, theApp->sequencedDataCollection());

	model->setStorageAdapter(storageAdapter);

	m_crawlingTableView->setModel(model);
	m_crawlingTableView->setViewModel(modelView);
	m_crawlingTableView->setShowAdditionalGrid(true);
	m_crawlingTableView->setContextMenu(new CommandMenu(storageAdapter));

	m_pageDataWidget = new PageDataWidget(this);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksOnThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksToThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::ServerResponseForPageType);

	m_splitter = new PageDataWidgetSplitter(this, m_crawlingTableView, m_pageDataWidget);
	layout->addWidget(m_splitter);

	VERIFY(connect(m_crawlingTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		m_pageDataWidget, SLOT(pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(m_crawlingTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&))));

	createHeaderActionWidgets();
}

void AllPagesPage::pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&)
{
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

void AllPagesPage::onApplySearch(int searchKey, const QString& searchValue)
{
	QSortFilterProxyModel* filterProxyModel = qobject_cast<QSortFilterProxyModel*>(m_crawlingTableView->model());
	ASSERT(filterProxyModel);

	filterProxyModel->setFilterKeyColumn(searchKey + 1);
	filterProxyModel->setFilterRegExp("^.*" + searchValue + ".*$");
}

void AllPagesPage::createHeaderActionWidgets()
{
	m_lookupLineEditWidget = new LookupLineEditWidget;
	addWidget(m_lookupLineEditWidget);

	const QVector<ParsedPageInfo::Column> columns =
		StorageAdapterFactory::parsedPageAvailableColumns(StorageAdapterType::StorageAdapterTypeAllPages);

	for (int i = 0; i < columns.size(); ++i)
	{
		m_lookupLineEditWidget->addSearchField(i, ParsedPageInfo::itemTypeDescription(columns[i]));
	}

	VERIFY(connect(m_lookupLineEditWidget, SIGNAL(applySearch(int, const QString&)),
		this, SLOT(onApplySearch(int, const QString&))));
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
