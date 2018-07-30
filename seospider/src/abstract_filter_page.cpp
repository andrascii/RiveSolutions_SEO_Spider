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
#include "preferences.h"
#include "action_registry.h"
#include "action_keys.h"
#include "main_window.h"
#include "content_frame.h"
#include "filter_table_selection_model.h"
#include "filter_info_widget.h"
#include "storage_exporter.h"
#include "crawler.h"
#include "lookup_lineedit_widget.h"
#include "parsed_page_info.h"
#include "storage_adapter_factory.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

AbstractFilterPage::AbstractFilterPage(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_webSiteDataWidget(webSiteDataWidget)
	, m_summaryFilterTableView(new TableView(this, true, false))
	, m_summaryFilterModel(new SummaryModel(this))
	, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterTableView, m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
	, m_isFirstShow(true)
	, m_info(new FilterInfoWidget(this))
	, m_lookupLineEditWidget(nullptr)
	, m_currentSelectedRow(-1)
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

void AbstractFilterPage::setInfoCategory(StorageAdapterType category)
{
	FilterInfoFactory infoFactory;
	std::optional<FilterInfo> filterInfo = infoFactory.filterInfo(category);

	if (filterInfo != std::nullopt)
	{
		m_info->setFilterInfo(filterInfo.value());
	}

	m_info->setVisible(filterInfo != std::nullopt);
	m_webSiteDataWidget->setStorageAdapterType(category);
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
	DEBUG_ASSERT(m_lookupLineEditWidget);

	if (!m_lookupLineEditWidget)
	{
		return;
	}

	m_lookupLineEditWidget->reset();
	m_lookupLineEditWidget->setEnabled(true);

	const QVector<ParsedPageInfo::Column> columns = 
		StorageAdapterFactory::parsedPageAvailableColumns(m_summaryFilterModel->storageAdapterType(m_summaryFilterModel->index(row, 0)));

	for (int i = 0; i < columns.size(); ++i)
	{
		m_lookupLineEditWidget->addSearchField(i, ParsedPageInfo::itemTypeDescription(columns[i]));
	}

	if (m_searchRules.contains(row))
	{
		onApplySearch(m_searchRules[row].searchKey, m_searchRules[row].searchData);
		m_lookupLineEditWidget->setCurrentSearchData(m_searchRules[row].searchKey, m_searchRules[row].searchData);
	}
}

void AbstractFilterPage::hasNoFilterSelection()
{
	DEBUG_ASSERT(m_lookupLineEditWidget);

	if (!m_lookupLineEditWidget)
	{
		return;
	}

	m_lookupLineEditWidget->setEnabled(false);
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

	setInfoCategory(summaryFilterModel()->storageAdapterType(index));

	const bool selectionExists = !indexes.isEmpty();

	if (selectionExists)
	{
		const int row = index.row();

		const auto uniqueRowNumberPredicate = [row](const QModelIndex& modelIndex)
		{
			return modelIndex.row() == row;
		};

		ASSERT(std::all_of(indexes.begin(), indexes.end(), uniqueRowNumberPredicate));

		m_currentSelectedRow = row;

		AbstractFilterPage::hasFilterSelection(row);
		hasFilterSelection(row);
	}
	else
	{
		m_currentSelectedRow = -1;

		AbstractFilterPage::hasNoFilterSelection();
		hasNoFilterSelection();
	}
}

void AbstractFilterPage::initHeaderWidgets()
{
	// this function must not be called from AbstractFilterPage constructor
	// in order to avoid crash with pure virtual function
	m_lookupLineEditWidget = new LookupLineEditWidget;
	addWidget(m_lookupLineEditWidget);

	VERIFY(connect(m_lookupLineEditWidget, SIGNAL(applySearch(int, const QString&)),
		this, SLOT(onApplySearch(int, const QString&))));
}

void AbstractFilterPage::exportFilterData()
{
	QAction* action = qobject_cast<QAction*>(sender());
	ASSERT(action && "This method must be called using QAction");

	const QVariant objectData = action->data();
	ASSERT(objectData.isValid() && "No data passed");

	std::vector<DCStorageDescription> storages;
	storages.push_back(qvariant_cast<DCStorageDescription>(objectData));

	StorageExporter::exportStorage(theApp->crawler()->sequencedDataCollection(), storages);
}

void AbstractFilterPage::reinitFilterTableSpans()
{
	m_summaryFilterTableView->initSpans();
}

void AbstractFilterPage::onApplySearch(int searchKey, const QString& searchValue)
{
	StorageAdapterType storageAdapterType =
		m_summaryFilterModel->storageAdapterType(m_summaryFilterModel->index(m_currentSelectedRow, 0));

	QSortFilterProxyModel* filterProxyModel =
		qobject_cast<QSortFilterProxyModel*>(websiteDataWidget()->modelFor(storageAdapterType));

	ASSERT(filterProxyModel);

	filterProxyModel->setFilterKeyColumn(searchKey + 1);
	filterProxyModel->setFilterRegExp("^.*" + searchValue + ".*$");

	m_searchRules[m_currentSelectedRow] = SearchRules{ searchKey, searchValue };

	if (!filterProxyModel->rowCount() && !searchValue.isEmpty())
	{
		websiteDataWidget()->showNoResultsLabelFor(AbstractPage::s_noResultsMessageStub.arg(searchValue));
	}

	if (searchValue.isEmpty())
	{
		websiteDataWidget()->hideNoResultsLabel();
	}
}

}