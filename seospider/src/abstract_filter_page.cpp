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

namespace SeoSpider
{

AbstractFilterPage::AbstractFilterPage(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_webSiteDataWidget(webSiteDataWidget)
	, m_summaryFilterTableView(new TableView(this, true, false))
	, m_summaryFilterModel(new SummaryModel(this))
	, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterTableView, m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
	, m_isFirstShow(true)
	, m_info(new FilterInfoWidget(this))
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
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();
	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, sequencedDataCollection);

	m_summaryFilterModel->setDataAccessor(summaryDataAccessor);
	m_summaryFilterTableView->initSpans();
	m_summaryFilterTableView->setContextMenu(new CommandMenu(summaryDataAccessor));
	m_summaryFilterTableView->setSelectionModel(new FilterTableSelectionModel(m_summaryFilterTableView->model()));

	VERIFY(connect(m_summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(summaryDataAccessor->qobject(), SIGNAL(dataSetChanged()), this, SLOT(reinitFilterTableSpans())));
}

void AbstractFilterPage::selectFilter(CrawlerEngine::StorageType type) const
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
		pageDataType < PageDataWidget::EndType)

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

		hasFilterSelection(row);
	}
	else
	{
		hasNoFilterSelection();
	}
}

void AbstractFilterPage::reinitFilterTableSpans()
{
	m_summaryFilterTableView->initSpans();
}

}