#include "abstract_filter_page.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "website_data_widget.h"
#include "helpers.h"
#include "application.h"
#include "command_menu.h"
#include "page_data_widget.h"
#include "filter_info_factory.h"
#include "filter_table_selection_model.h"
#include "filter_info_widget.h"
#include "storage_exporter.h"
#include "lookup_lineedit_widget.h"
#include "columns_lookup_lineedit_widget.h"
#include "parsed_page_info.h"
#include "storage_adapter_factory.h"
#include "storage_adapter_type.h"
#include "statistic_counter.h"
#include "table_proxy_model.h"
#include "resource_type_filter_widget.h"
#include "supported_resource_type_filtering_storages.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

AbstractFilterPage::AbstractFilterPage(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: AbstractTablePage(parent, webSiteDataWidget->pageDataWidget())
	, m_webSiteDataWidget(webSiteDataWidget)
	, m_summaryFilterTableView(new TableView(this, true, false, false))
	, m_summaryFilterModel(new SummaryModel(this))
	, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterTableView, m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
	, m_isFirstShow(true)
	, m_info(new FilterInfoWidget(this))
	, m_columnsLookupLineEditWidget(nullptr)
	, m_lookupLineEditWidget(nullptr)
	, m_currentSelectedRow(-1)
	, m_currentStorageType(CrawlerEngine::StorageType::BeginEnumStorageType)
{
	m_summaryFilterTableView->setModel(m_summaryFilterModel);
	m_summaryFilterTableView->setViewModel(m_summaryFilterViewModel);
	m_summaryFilterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryFilterTableView->horizontalHeader()->hide();
	m_summaryFilterTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_summaryFilterTableView->setObjectName("FilterWidgetTableView");

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	QWidget* tableViewAndInfo = new QWidget(this);
	tableViewAndInfo->setLayout(vLayout);
	vLayout->addWidget(m_info);
	vLayout->addWidget(m_webSiteDataWidget);
	m_info->setVisible(false);

	m_splitter->setOrientation(Qt::Horizontal);
	m_splitter->setChildrenCollapsible(false);

	m_splitter->addWidget(m_summaryFilterTableView);
	m_splitter->addWidget(tableViewAndInfo);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(m_splitter);

	VERIFY(connect(theApp, &Application::mainWindowShown, this, &AbstractFilterPage::adjustSize));
}

void AbstractFilterPage::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType)
{
	SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();
	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, sequencedDataCollection);

	m_summaryFilterModel->setDataAccessor(summaryDataAccessor);
	m_summaryFilterTableView->initSpans();
	m_summaryFilterTableView->setContextMenu(new CommandMenu(summaryDataAccessor));
	m_summaryFilterTableView->setSelectionModel(new FilterTableSelectionModel(m_summaryFilterTableView->model()));

	VERIFY(connect(m_summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(summaryDataAccessor->qobject(), SIGNAL(dataSetChanged()), this, SLOT(reinitFilterTableSpans())));
}

void AbstractFilterPage::selectFilter(StorageType type) const
{
	const int row = m_summaryFilterModel->dataAccessor()->rowByStorageType(type);

	m_summaryFilterTableView->selectionModel()->select(
		m_summaryFilterModel->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
	);
}

void AbstractFilterPage::selectParsedPage(int row)
{
	m_webSiteDataWidget->selectParsedPage(row);
}

void AbstractFilterPage::selectTab(int pageDataType)
{
	ASSERT(pageDataType > PageDataWidget::BeginType &&
		pageDataType < PageDataWidget::EndType);

	if (!m_webSiteDataWidget->pageDataWidget())
	{
		return;
	}

	m_webSiteDataWidget->pageDataWidget()->selectTab(static_cast<PageDataWidget::PageDataType>(pageDataType));
}

void AbstractFilterPage::enableSortableFilter()
{
	ASSERT(m_summaryFilterModel->dataAccessor()->hasSortableDataSet());

	m_summaryFilterModel->dataAccessor()->enableSortableDataSet();
}

void AbstractFilterPage::enablePlainFilter()
{
	m_summaryFilterModel->dataAccessor()->enablePlainDataSet();
}

void AbstractFilterPage::setInfoCategory(StorageAdapterType category, const QString& customDataFeed)
{
	FilterInfoFactory infoFactory;
	std::optional<FilterInfo> filterInfo = infoFactory.filterInfo(category);

	if (filterInfo != std::nullopt)
	{
		m_info->setFilterInfo(filterInfo.value());
	}

	m_info->setVisible(filterInfo != std::nullopt);
	m_webSiteDataWidget->setStorageAdapterType(category, customDataFeed);
}

const SummaryModel* AbstractFilterPage::summaryFilterModel() const
{
	return m_summaryFilterModel;
}

WebSiteDataWidget* AbstractFilterPage::websiteDataWidget()
{
	return m_webSiteDataWidget;
}

void AbstractFilterPage::showEvent(QShowEvent* event)
{
	m_summaryFilterTableView->recalculateColumnsSize();
	QFrame::showEvent(event);
}

void AbstractFilterPage::hasFilterSelection(int row)
{
	Q_UNUSED(row);
	preparePlainSearchWidget();
}

void AbstractFilterPage::hasNoFilterSelection()
{
	if (m_columnsLookupLineEditWidget)
	{
		m_columnsLookupLineEditWidget->setEnabled(false);
	}

	if (m_lookupLineEditWidget)
	{
		m_lookupLineEditWidget->setEnabled(false);
	}
}

void AbstractFilterPage::adjustSize()
{
	QWidget* parentWidget = qobject_cast<QWidget*>(parent());

	ASSERT(parentWidget);

	const int summaryViewWidth = Common::Helpers::pointsToPixels(180);
	const int parentWidgetWidth = parentWidget->width();
	m_splitter->setSizes(QList<int>() << summaryViewWidth << parentWidgetWidth - summaryViewWidth);
}

void AbstractFilterPage::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);

	const QModelIndexList indexes = selected.indexes();
	const QModelIndex index = !indexes.isEmpty() ? indexes[0] : QModelIndex();

	setInfoCategory(summaryFilterModel()->storageAdapterType(index), summaryFilterModel()->customDataFeed(index));

	const bool selectionExists = !indexes.isEmpty();

	if (selectionExists)
	{
		const int row = index.row();

		const auto uniqueRowNumberPredicate = [row](const QModelIndex& modelIndex)
		{
			return modelIndex.row() == row;
		};

		ASSERT(std::all_of(indexes.begin(), indexes.end(), uniqueRowNumberPredicate));

		// TODO: remove this code if you delete sortable data set from the program!!!

		const int oldSelectedRow = m_currentSelectedRow;

		// avoid calculating new and old selected storage by row
		// because it's a resource-intensive operation
		if (m_currentSelectedRow == row)
		{
			return;
		}
		else
		{
			// we must update current row every time to have ability to apply search rules
			// even if the current row was changed but current storage type stays the same
			// this is possible when we enable sortable data set (in this case row can be changed but storage type may stay the same)
			// if we forget to do it then debug assertion will crash our program at the applying search rules stage if enabled sortable data set
			setCurrentStorageDescriptors(row, m_currentStorageType);
		}

		const DCStorageDescription* newSelectedStorage = summaryFilterModel()->dataAccessor()->storageDescriptionByRow(row);

		// in order to prevent clearing lookup line edit widget data
		if (newSelectedStorage && newSelectedStorage->storageType == m_currentStorageType)
		{
			// TODO: validate applied search rules to the new index but how to do that?
			return;
		}

		setCurrentStorageDescriptors(row, newSelectedStorage ? newSelectedStorage->storageType : CrawlerEngine::StorageType::BeginEnumStorageType);

		const auto typeString = storageAdapterTypeStringFromEnum(summaryFilterModel()->storageAdapterType(index));
		Common::StatisticCounter showCounter(QString("%1_%2FilterShowCounter").arg(pageName()).arg(typeString));
		showCounter.increment();

		AbstractFilterPage::hasFilterSelection(row);
		hasFilterSelection(row);
		updateResourceTypeFilterWidget();
	}
	else
	{
		resetCurrentStorageDescriptors();

		AbstractFilterPage::hasNoFilterSelection();
		hasNoFilterSelection();
	}
}

void AbstractFilterPage::applySearchHelper(int searchColumnNumber, const QString& searchValue)
{
	if (m_currentSelectedRow == -1)
	{
		return;
	}

	StorageAdapterType storageAdapterType =
		m_summaryFilterModel->storageAdapterType(m_summaryFilterModel->index(m_currentSelectedRow, 0));

	QSortFilterProxyModel* filterProxyModel =
		qobject_cast<QSortFilterProxyModel*>(websiteDataWidget()->modelFor(storageAdapterType));

	ASSERT(filterProxyModel);

	filterProxyModel->setFilterKeyColumn(searchColumnNumber);
	filterProxyModel->setFilterRegExp("^.*" + searchValue + ".*$");

	if (!filterProxyModel->rowCount() && !searchValue.isEmpty())
	{
		websiteDataWidget()->showNoResultsLabelFor(AbstractPage::s_noResultsMessageStub.arg(searchValue));
	}
	else
	{
		websiteDataWidget()->hideNoResultsLabel();
	}
}

void AbstractFilterPage::initHeaderWidgets()
{
	// this function must not be called from AbstractFilterPage constructor
	// in order to avoid crash with pure virtual function

	m_columnsLookupLineEditWidget = new ColumnsLookupLineEditWidget;
	m_columnsLookupLineEditWidget->setProperty("ReservedForFuture", true);

	addWidget(m_columnsLookupLineEditWidget);

	VERIFY(connect(m_columnsLookupLineEditWidget, SIGNAL(applySearch(int, const QString&)),
		this, SLOT(onApplyColumnSearch(int, const QString&))));

	m_columnsLookupLineEditWidget->setEnabled(false);

	//////////////////////////////////////////////////////////////////////////

	m_resourceTypeFilterWidget = new ResourceTypeFilterWidget();
	addWidget(m_resourceTypeFilterWidget);
	m_resourceTypeFilterWidget->setVisible(false);
	m_resourceTypeFilterWidget->installEventFilter(this);

	m_lookupLineEditWidget = new LookupLineEditWidget;
	addWidget(m_lookupLineEditWidget);

	m_lookupLineEditWidget->setEnabled(false);

	VERIFY(connect(m_resourceTypeFilterWidget, &ResourceTypeFilterWidget::filterChanged,
		this, &AbstractFilterPage::onResourceTypeFilterChanged));

	VERIFY(connect(m_lookupLineEditWidget, SIGNAL(applySearch(const QString&)),
		this, SLOT(onApplyPlainSearch(const QString&))));
}

bool AbstractFilterPage::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_resourceTypeFilterWidget && event->type() == QEvent::Show)
	{
		updateResourceTypeFilterWidget();
	}

	return false;
}

void AbstractFilterPage::reinitFilterTableSpans()
{
	m_summaryFilterTableView->initSpans();
}

void AbstractFilterPage::onApplyColumnSearch(int searchKey, const QString& searchValue)
{
	applySearchHelper(searchKey + 1, searchValue);
	m_searchRules[m_currentStorageType] = SearchRules{ searchKey, searchValue };
}

void AbstractFilterPage::onApplyPlainSearch(const QString& searchValue)
{
	applySearchHelper(-1, searchValue);
	m_searchRules[m_currentStorageType] = SearchRules{ -1, searchValue };
}

void AbstractFilterPage::onResourceTypeFilterChanged(int filter)
{
	StorageAdapterType storageAdapterType =
		m_summaryFilterModel->storageAdapterType(m_summaryFilterModel->index(m_currentSelectedRow, 0));

	if (storageAdapterType == StorageAdapterType::StorageAdapterTypeNone)
	{
		return;
	}

	TableProxyModel* filterProxyModel =
		qobject_cast<TableProxyModel*>(websiteDataWidget()->modelFor(storageAdapterType));

	ASSERT(filterProxyModel);
	filterProxyModel->setAcceptedResources(filter);
}

void AbstractFilterPage::prepareColumnSearchWidget(int row)
{
	if (!m_columnsLookupLineEditWidget)
	{
		return;
	}

	m_columnsLookupLineEditWidget->reset();
	m_columnsLookupLineEditWidget->setEnabled(true);

	const QVector<ParsedPageInfo::Column> columns =
		StorageAdapterFactory::parsedPageAvailableColumns(m_summaryFilterModel->storageAdapterType(m_summaryFilterModel->index(row, 0)));

	for (int i = 0; i < columns.size(); ++i)
	{
		m_columnsLookupLineEditWidget->addSearchField(i, ParsedPageInfo::itemTypeDescription(columns[i]));
	}

	if (m_searchRules.contains(m_currentStorageType))
	{
		onApplyColumnSearch(m_searchRules[m_currentStorageType].searchKey, m_searchRules[m_currentStorageType].searchData);
		m_columnsLookupLineEditWidget->setCurrentSearchData(m_searchRules[m_currentStorageType].searchKey, m_searchRules[m_currentStorageType].searchData);
	}
}

void AbstractFilterPage::preparePlainSearchWidget()
{
	if (!m_lookupLineEditWidget)
	{
		return;
	}

	m_lookupLineEditWidget->reset();
	m_lookupLineEditWidget->setEnabled(true);

	if (m_searchRules.contains(m_currentStorageType))
	{
		onApplyPlainSearch(m_searchRules[m_currentStorageType].searchData);
		m_lookupLineEditWidget->setCurrentSearchData(m_searchRules[m_currentStorageType].searchData);
	}
}

void AbstractFilterPage::updateResourceTypeFilterWidget()
{
	StorageAdapterType storageAdapterType =
		m_summaryFilterModel->storageAdapterType(m_summaryFilterModel->index(m_currentSelectedRow, 0));

	if (!storageSupportsResourceTypeFiltering(storageAdapterType))
	{
		m_resourceTypeFilterWidget->setVisible(false);
		return;
	}

	// TODO: hide or show filter widget depending on current storage
	m_resourceTypeFilterWidget->setVisible(true);
	TableProxyModel* filterProxyModel =
		qobject_cast<TableProxyModel*>(websiteDataWidget()->modelFor(storageAdapterType));

	ASSERT_WITH_DUMP(filterProxyModel, DUMP(m_currentSelectedRow), DUMP((int)storageAdapterType));
	m_resourceTypeFilterWidget->setFilter(filterProxyModel->acceptedResources());
}

void AbstractFilterPage::setCurrentStorageDescriptors(int row, CrawlerEngine::StorageType storageType)
{
	m_currentSelectedRow = row;
	m_currentStorageType = storageType;
}

void AbstractFilterPage::resetCurrentStorageDescriptors()
{
	m_currentSelectedRow = -1;
	m_currentStorageType = CrawlerEngine::StorageType::BeginEnumStorageType;
}

}
