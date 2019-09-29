#include "data_extraction_page.h"
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
#include "svg_renderer.h"

namespace SeoSpider
{

DataExtractionPage::DataExtractionPage(QWidget* parent)
	: AbstractTablePage(parent, new PageDataWidget())
	, m_stackedTableView(new QStackedWidget(this))
	, m_splitter(nullptr)
	, m_columnsLookupLineEditWidget(nullptr)
	, m_lookupLineEditWidget(nullptr)
	, m_exportFilterDataAction(nullptr)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	TableView* tableView = new TableView(this, false, true, true);
	PageModel* model = new PageModel;
	PageViewModel* modelView = new PageViewModel(tableView, model, devicePixelRatio());

	IStorageAdapter* storageAdapter = theApp->storageAdapterFactory()->createParsedPageInfoStorage(
		StorageAdapterType::StorageAdapterTypeDataExtraction, theApp->sequencedDataCollection());

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

void DataExtractionPage::pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&)
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

void DataExtractionPage::onApplyColumnSearch(int searchKey, const QString& searchValue)
{
	applySearchHelper(searchKey + 1, searchValue);
}

void DataExtractionPage::onApplyPlainSearch(const QString& searchValue)
{
	applySearchHelper(-1, searchValue);
}

void DataExtractionPage::onResourceTypeFilterChanged(int filter)
{
	TableProxyModel* filterProxyModel = qobject_cast<TableProxyModel*>(tableView()->model());
	ASSERT(filterProxyModel);
	filterProxyModel->setAcceptedResources(filter);
}

void DataExtractionPage::createHeaderActionWidgets()
{
	m_columnsLookupLineEditWidget = new ColumnsLookupLineEditWidget;
	m_columnsLookupLineEditWidget->setProperty("ReservedForFuture", true);

	addWidget(m_columnsLookupLineEditWidget);

	const QVector<ParsedPageInfo::Column> columns =
		StorageAdapterFactory::defaultColumns(StorageAdapterType::StorageAdapterTypeAllPages);

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
		this, &DataExtractionPage::onResourceTypeFilterChanged));

	VERIFY(connect(m_lookupLineEditWidget, SIGNAL(applySearch(const QString&)),
		this, SLOT(onApplyPlainSearch(const QString&))));

	//////////////////////////////////////////////////////////////////////////

	m_exportFilterDataAction = new QAction(SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20),
		tr("Export selected filter data to .xlsx file"), this);

	AbstractPage::addAction(m_exportFilterDataAction);

	VERIFY(connect(m_exportFilterDataAction, &QAction::triggered, this, &DataExtractionPage::exportFilterData));

	static DCStorageDescription s_description
	{
		CrawlerEngine::StorageType::CrawledUrlStorageType,
		name(),
	};

	m_exportFilterDataAction->setData(QVariant::fromValue(s_description));
}

void DataExtractionPage::showNoResultsLabelFor(const QString& searchValue)
{
	QLabel* noResultsLabel =
		qobject_cast<QLabel*>(m_stackedTableView->widget(m_widgetIndexes[NoResultsWidget]));

	ASSERT(noResultsLabel);

	noResultsLabel->setText(s_noResultsMessageStub.arg(searchValue));
	m_stackedTableView->setCurrentIndex(m_widgetIndexes[NoResultsWidget]);
}

void DataExtractionPage::hideNoResultsLabel()
{
	m_stackedTableView->setCurrentIndex(m_widgetIndexes[CrawlingTableViewWidget]);
}

void DataExtractionPage::applySearchHelper(int searchColumnNumber, const QString& searchValue)
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

QWidget* DataExtractionPage::widget() const
{
	return const_cast<DataExtractionPage*>(this);
}

QString DataExtractionPage::name() const
{
	return tr("Data Extraction");
}

QIcon DataExtractionPage::icon() const
{
	QIcon icon;
	icon.addPixmap(SvgRenderer::render(":/images/data-extraction-page-icon.svg", 15, 15), QIcon::Normal, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/data-extraction-page-icon.svg", 15, 15), QIcon::Active, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/data-extraction-page-icon.svg", 15, 15), QIcon::Active, QIcon::Off);
	icon.addPixmap(SvgRenderer::render(":/images/data-extraction-page-icon.svg", 15, 15), QIcon::Normal, QIcon::Off);

	return icon;
}

IPage::Type DataExtractionPage::type() const
{
	return IPage::DataExtractionPage;
}

TableView* DataExtractionPage::tableView() const
{
	TableView* tableView = qobject_cast<TableView*>(
		m_stackedTableView->widget(m_widgetIndexes[CrawlingTableViewWidget]));

	ASSERT(tableView);
	return tableView;
}

void DataExtractionPage::showEvent(QShowEvent * event)
{
	QFrame::showEvent(event);

	Common::StatisticCounter showCounter("AllSitePagesShowCounter");
	showCounter.increment();
}

}
