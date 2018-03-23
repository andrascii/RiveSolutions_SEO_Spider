#include "filter_widget.h"
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

namespace SeoSpider
{

FilterInfoWidget::FilterInfoWidget(QWidget* parent)
	: QFrame(parent)
	, m_title(new QLabel(this))
	, m_description(new QLabel(this))
{
	m_title->setObjectName(QString("filterInfoWidgetTitle"));
	m_description->setObjectName(QString("filterInfoWidgetDescription"));

	QVBoxLayout* layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->addWidget(m_title);
	layout->addWidget(m_description);

	setObjectName(QString("filterInfo"));
}

QLabel* FilterInfoWidget::title() const
{
	return m_title;
}

QLabel* FilterInfoWidget::description() const
{
	return m_description;
}

FilterWidget::FilterWidget(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_webSiteDataWidget(webSiteDataWidget)
	//, m_summaryFilterTableViews(new QMap<SummaryDataAccessorFactory::DataAccessorType, TableView*>)
	//, m_summaryFilterModels(new QMap<SummaryDataAccessorFactory::DataAccessorType, SummaryModel*>)
	//, m_summaryFilterViewModels(new QMap<SummaryDataAccessorFactory::DataAccessorType, SummaryViewModel*>)
	, m_stackedFilterWidget(new QStackedWidget(this))
	//, m_summaryFilterTableView(new TableView(this, true))
	//, m_summaryFilterModel(new SummaryModel(this))
	//, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
	, m_isFirstShow(true)
	, m_info(new FilterInfoWidget(this))
{
	//m_summaryFilterTableView->setModel(m_summaryFilterModel);
	//m_summaryFilterTableView->setViewModel(m_summaryFilterViewModel);
	//m_summaryFilterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	//m_summaryFilterTableView->horizontalHeader()->hide();
	//m_summaryFilterTableView->setObjectName("FilterWidgetTableView");

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	QWidget* tableViewAndInfo = new QWidget(this);
	tableViewAndInfo->setLayout(vLayout);
	vLayout->addWidget(m_info);
	vLayout->addWidget(m_webSiteDataWidget);
	m_info->setVisible(false);

	m_splitter->setOrientation(Qt::Horizontal);
	m_splitter->setChildrenCollapsible(false);

	m_splitter->addWidget(m_stackedFilterWidget);
	//m_splitter->addWidget(m_webSiteDataWidget);
	//m_splitter->addWidget(m_summaryFilterTableView);
	m_splitter->addWidget(tableViewAndInfo);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(m_splitter);

	//VERIFY(connect(m_summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
	//	this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(theApp, &Application::mainWindowShown, this, &FilterWidget::adjustSize));
}

void FilterWidget::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType) 
{
	createSummaryFilterTable(dataAccessorType);

	CrawlerEngine::SequencedDataCollection* guiStorage = theApp->sequencedDataCollection();
	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, guiStorage);
	m_summaryFilterModels[dataAccessorType]->setDataAccessor(summaryDataAccessor);
	m_summaryFilterTableViews[dataAccessorType]->initSpans();
	m_summaryFilterTableViews[dataAccessorType]->setContextMenu(new CommandMenu(summaryDataAccessor));
	//selectFilter(CrawlerEngine::StorageType::HtmlResourcesStorageType);
	
	m_stackedFilterWidget->addWidget(m_summaryFilterTableViews[dataAccessorType]);
	m_stackedFilterWidget->setCurrentIndex(0);
}

void FilterWidget::selectFilter(CrawlerEngine::StorageType type) const
{
	const int row = m_summaryFilterModels[currentKey()]->dataAccessor()->rowByStorageType(type);
	m_summaryFilterTableViews[currentKey()]->selectionModel()->select(m_summaryFilterModels[currentKey()]->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void FilterWidget::selectParsedPage(int row)
{
	m_webSiteDataWidget->selectParsedPage(row);
}

void FilterWidget::selectTab(int pageDataType)
{
	ASSERT(pageDataType > PageDataWidget::BeginType && 
		pageDataType < PageDataWidget::EndType)
	
	if(!m_webSiteDataWidget->pageDataWidget())
	{
		return;
	}

	m_webSiteDataWidget->pageDataWidget()->selectTab(static_cast<PageDataWidget::PageDataType>(pageDataType));
}

void FilterWidget::groupByErrorType()
{
	//m_stackedFilterWidget->currentIndex() == 0 ? m_stackedFilterWidget->setCurrentIndex(1) : m_stackedFilterWidget->setCurrentIndex(0);

	if(m_stackedFilterWidget->currentIndex() == 0)
	{
		m_stackedFilterWidget->setCurrentIndex(1);
	}
	else
	{
		m_stackedFilterWidget->setCurrentIndex(0);
	}
}

void FilterWidget::createSummaryFilterTable(SummaryDataAccessorFactory::DataAccessorType dataAccessorType)
{
	TableView* summaryFilterTableView = new TableView(this, true);
	SummaryModel* summaryFilterModel = new SummaryModel(this);
	SummaryViewModel* summaryFilterViewModel = new SummaryViewModel(summaryFilterModel, this);

	m_summaryFilterModels[dataAccessorType] = summaryFilterModel;
	m_summaryFilterViewModels[dataAccessorType] = summaryFilterViewModel;
	
	summaryFilterTableView->setModel(m_summaryFilterModels[dataAccessorType]);
	summaryFilterTableView->setViewModel(m_summaryFilterViewModels[dataAccessorType]);
	summaryFilterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	summaryFilterTableView->horizontalHeader()->hide();
	summaryFilterTableView->setObjectName("FilterWidgetTableView");

	m_summaryFilterTableViews[dataAccessorType] = summaryFilterTableView;

	VERIFY(connect(summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

}

SummaryDataAccessorFactory::DataAccessorType FilterWidget::currentKey() const
{
	return m_summaryFilterTableViews.key(dynamic_cast<TableView*>(m_stackedFilterWidget->currentWidget()));
}

void FilterWidget::adjustSize()
{
	QWidget* parentWidget = qobject_cast<QWidget*>(parent());

	ASSERT(parentWidget);

	const int summaryViewWidth = Common::Helpers::pointsToPixels(180);
	const int parentWidgetWidth = parentWidget->width();
	m_splitter->setSizes(QList<int>() << summaryViewWidth << parentWidgetWidth - summaryViewWidth);
}

void FilterWidget::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) const
{
	Q_UNUSED(deselected);

	const QModelIndex index = selected.size() ? selected.indexes()[0] : QModelIndex();
	const StorageAdapterType category = m_summaryFilterModels[currentKey()]->storageAdapterType(index);

	FilterInfoFactory infoFactory;
	std::optional<FilterInfo> filterInfo = infoFactory.filterInfo(category);

	if (filterInfo != std::nullopt)
	{
		m_info->title()->setText(filterInfo->title);

		QString description = filterInfo->description;

		foreach(const QByteArray& prop, filterInfo->props)
		{
			description = description.arg(theApp->preferences()->property(prop).toString());
		}

		m_info->description()->setText(description);
	}

	m_info->setVisible(filterInfo != std::nullopt);
	m_webSiteDataWidget->setStorageAdapterType(category);


	QByteArray exportFilterDataActionKey;
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	if (qobject_cast<const QWidget*>(this) == theApp->mainWindow()->contentFrame()->page(PageFactory::Page::SiteAuditPage))
	{
		exportFilterDataActionKey = s_exportFilterDataAuditPageAction;
	}
	else if (qobject_cast<const QWidget*>(this) == theApp->mainWindow()->contentFrame()->page(PageFactory::Page::AllResourcesPage))
	{
		exportFilterDataActionKey = s_exportFilterDataAllResourcesPageAction;
	}

	if (!exportFilterDataActionKey.isEmpty())
	{
		const bool hasSelection = m_summaryFilterTableViews[currentKey()]->selectionModel()->hasSelection();

		if (hasSelection)
		{
			QModelIndexList modelIndexes = selected.indexes();
			const int row = modelIndexes[0].row();

			const auto uniqueRowNumberPredicate = [row](const QModelIndex& modelIndex) 
			{ 
				return modelIndex.row() == row;
			};

			ASSERT(std::all_of(modelIndexes.begin(), modelIndexes.end(), uniqueRowNumberPredicate));

			const DCStorageDescription* storageDescription = m_summaryFilterModels[currentKey()]->dataAccessor()->storageDescriptionByRow(row);
			actionRegistry.globalAction(exportFilterDataActionKey)->setData(QVariant::fromValue(*storageDescription));
		}

		actionRegistry.globalAction(exportFilterDataActionKey)->setEnabled(hasSelection);
	}
}

}
