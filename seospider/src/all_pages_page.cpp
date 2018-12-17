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
#include "columns_lookup_lineedit_widget.h"
#include "statistic_counter.h"
#include "resource_type_filter_widget.h"
#include "table_proxy_model.h"

namespace SeoSpider
{

AllPagesPage::AllPagesPage(QWidget* parent)
	: QFrame(parent)
	, AbstractTablePage(new PageDataWidget(this))
	, m_stackedTableView(new QStackedWidget(this))
	, m_splitter(nullptr)
	, m_columnsLookupLineEditWidget(nullptr)
	, m_lookupLineEditWidget(nullptr)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	TableView* tableView = new TableView(this, false, true);
	PageModel* model = new PageModel;
	PageViewModel* modelView = new PageViewModel(tableView, model);

	IStorageAdapter* storageAdapter = theApp->storageAdapterFactory()->createParsedPageInfoStorage(
		StorageAdapterType::StorageAdapterTypeAllPages, theApp->sequencedDataCollection());

	model->setStorageAdapter(storageAdapter);

	tableView->setModel(model);
	tableView->setViewModel(modelView);
	tableView->setShowAdditionalGrid(true);
	tableView->setContextMenu(new CommandMenu(storageAdapter));

	m_pageDataWidget->setPageDataType(PageDataWidget::LinksOnThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksToThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::ServerResponseForPageType);

	QLabel* noSearchResultsLabel = new QLabel(this);
	noSearchResultsLabel->setObjectName(QStringLiteral("NoSearchResultsLabel"));
	noSearchResultsLabel->setText(AbstractPage::s_noResultsMessageStub);
	noSearchResultsLabel->setAlignment(Qt::AlignCenter);
	noSearchResultsLabel->setWordWrap(true);

	m_widgetIndexes[NoResultsWidget] = m_stackedTableView->addWidget(noSearchResultsLabel);
	m_widgetIndexes[CrawlingTableViewWidget] = m_stackedTableView->addWidget(tableView);
	m_stackedTableView->setCurrentIndex(m_widgetIndexes[CrawlingTableViewWidget]);

	m_splitter = new PageDataWidgetSplitter(this, m_stackedTableView, m_pageDataWidget);
	layout->addWidget(m_splitter);

	VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		m_pageDataWidget, SLOT(pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
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

void AllPagesPage::onApplyColumnSearch(int searchKey, const QString& searchValue)
{
	applySearchHelper(searchKey + 1, searchValue);
}

void AllPagesPage::onApplyPlainSearch(const QString& searchValue)
{
	applySearchHelper(-1, searchValue);
}

void AllPagesPage::onResourceTypeFilterChanged(int filter)
{
	TableProxyModel* filterProxyModel = qobject_cast<TableProxyModel*>(tableView()->model());
	ASSERT(filterProxyModel);
	filterProxyModel->setAcceptedResources(filter);
}

void AllPagesPage::createHeaderActionWidgets()
{
	m_columnsLookupLineEditWidget = new ColumnsLookupLineEditWidget;
	m_columnsLookupLineEditWidget->setProperty("ReservedForFuture", true);

	addWidget(m_columnsLookupLineEditWidget);

	const QVector<ParsedPageInfo::Column> columns =
		StorageAdapterFactory::parsedPageAvailableColumns(StorageAdapterType::StorageAdapterTypeAllPages);

	for (int i = 0; i < columns.size(); ++i)
	{
		m_columnsLookupLineEditWidget->addSearchField(i, ParsedPageInfo::itemTypeDescription(columns[i]));
	}

	VERIFY(connect(m_columnsLookupLineEditWidget, SIGNAL(applySearch(int, const QString&)),
		this, SLOT(onApplyColumnSearch(int, const QString&))));

	//////////////////////////////////////////////////////////////////////////

	ResourceTypeFilterWidget* resourceFilter = new ResourceTypeFilterWidget();
	addWidget(resourceFilter);

	m_lookupLineEditWidget = new LookupLineEditWidget;
	addWidget(m_lookupLineEditWidget);

	VERIFY(connect(resourceFilter, &ResourceTypeFilterWidget::filterChanged, 
		this, &AllPagesPage::onResourceTypeFilterChanged));

	VERIFY(connect(m_lookupLineEditWidget, SIGNAL(applySearch(const QString&)),
		this, SLOT(onApplyPlainSearch(const QString&))));
}

void AllPagesPage::showNoResultsLabelFor(const QString& searchValue)
{
	QLabel* noResultsLabel =
		qobject_cast<QLabel*>(m_stackedTableView->widget(m_widgetIndexes[NoResultsWidget]));

	ASSERT(noResultsLabel);

	noResultsLabel->setText(s_noResultsMessageStub.arg(searchValue));
	m_stackedTableView->setCurrentIndex(m_widgetIndexes[NoResultsWidget]);
}

void AllPagesPage::hideNoResultsLabel()
{
	m_stackedTableView->setCurrentIndex(m_widgetIndexes[CrawlingTableViewWidget]);
}

void AllPagesPage::applySearchHelper(int searchColumnNumber, const QString& searchValue)
{
	QSortFilterProxyModel* filterProxyModel = qobject_cast<QSortFilterProxyModel*>(tableView()->model());
	ASSERT(filterProxyModel);

	filterProxyModel->setFilterKeyColumn(searchColumnNumber);
	filterProxyModel->setFilterRegExp("^.*" + searchValue + ".*$");

	if (!filterProxyModel->rowCount() && !searchValue.isEmpty())
	{
		showNoResultsLabelFor(searchValue);
	}
	else
	{
		hideNoResultsLabel();
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

TableView* AllPagesPage::tableView() const
{
	TableView* tableView = qobject_cast<TableView*>(
		m_stackedTableView->widget(m_widgetIndexes[CrawlingTableViewWidget]));

	ASSERT(tableView);
	return tableView;
}

void AllPagesPage::showEvent(QShowEvent * event)
{
	QFrame::showEvent(event);

	StatisticCounter showCounter("AllSitePagesShowCounter");
	showCounter.increment();
}

}
