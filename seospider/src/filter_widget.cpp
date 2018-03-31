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
#include "deferred_call.h"
#include "filter_table_selection_model.h"

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

	m_description->setWordWrap(true);

	setObjectName(QString("filterInfo"));
}

void FilterInfoWidget::setFilterInfo(const FilterInfo& filterInfo)
{
	m_filterInfo = filterInfo;

	Preferences* preferences = theApp->preferences();
	const QMetaObject& meta = preferences->staticMetaObject;

	disconnect(preferences);

	foreach(const QByteArray& prop, filterInfo.props)
	{
		for (int i = 0; i < meta.propertyCount(); ++i)
		{
			const QMetaProperty& metaProperty = meta.property(i);
			QByteArray iPropName = metaProperty.name();
			if (iPropName == prop)
			{
				QByteArray signal = "2" + metaProperty.notifySignal().methodSignature();
				VERIFY(connect(preferences, signal, this, SLOT(onPropertyChanged())));
			}
		}
	}

	onPropertyChanged();
}

void FilterInfoWidget::onPropertyChanged()
{
	Preferences* preferences = theApp->preferences();

	m_title->setText(m_filterInfo.title);

	QString description = m_filterInfo.description;

	foreach(const QByteArray& prop, m_filterInfo.props)
	{
		description = description.arg(preferences->property(prop).toString());
	}

	m_description->setText(description);
}

FilterWidget::FilterWidget(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_webSiteDataWidget(webSiteDataWidget)
	, m_summaryFilterTableView(new TableView(this, true))
	, m_summaryFilterModel(new SummaryModel(this))
	, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
	, m_isFirstShow(true)
	, m_info(new FilterInfoWidget(this))
{
	m_summaryFilterTableView->setModel(m_summaryFilterModel);
	m_summaryFilterTableView->setViewModel(m_summaryFilterViewModel);
	m_summaryFilterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryFilterTableView->horizontalHeader()->hide();
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

	VERIFY(connect(theApp, &Application::mainWindowShown, this, &FilterWidget::adjustSize));
}

void FilterWidget::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType) 
{
	CrawlerEngine::SequencedDataCollection* sequencedDataCollection = theApp->sequencedDataCollection();
	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, sequencedDataCollection);

	m_summaryFilterModel->setDataAccessor(summaryDataAccessor);
	m_summaryFilterTableView->initSpans();
	m_summaryFilterTableView->setContextMenu(new CommandMenu(summaryDataAccessor));
	m_summaryFilterTableView->setSelectionModel(new FilterTableSelectionModel(m_summaryFilterModel));

	VERIFY(connect(m_summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(summaryDataAccessor->qobject(), SIGNAL(dataSetChanged()), this, SLOT(reinitFilterTableSpans())));
}

void FilterWidget::selectFilter(CrawlerEngine::StorageType type) const
{
	const int row = m_summaryFilterModel->dataAccessor()->rowByStorageType(type);

	m_summaryFilterTableView->selectionModel()->select(
		m_summaryFilterModel->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
	);
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

void FilterWidget::enableSortableFilter()
{
	ASSERT(m_summaryFilterModel->dataAccessor()->hasSortableDataSet());

	m_summaryFilterModel->dataAccessor()->enableSortableDataSet();
}

void FilterWidget::enablePlainFilter()
{
	m_summaryFilterModel->dataAccessor()->enablePlainDataSet();
}

void FilterWidget::adjustSize()
{
	QWidget* parentWidget = qobject_cast<QWidget*>(parent());

	ASSERT(parentWidget);

	const int summaryViewWidth = Common::Helpers::pointsToPixels(180);
	const int parentWidgetWidth = parentWidget->width();
	m_splitter->setSizes(QList<int>() << summaryViewWidth << parentWidgetWidth - summaryViewWidth);
}

void FilterWidget::reinitFilterTableSpans()
{
	m_summaryFilterTableView->initSpans();
}

void FilterWidget::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);

	const QModelIndexList indexes = selected.indexes();
	const QModelIndex index = !indexes.isEmpty() ? indexes[0] : QModelIndex();

	const StorageAdapterType category = m_summaryFilterModel->storageAdapterType(index);

	FilterInfoFactory infoFactory;
	std::optional<FilterInfo> filterInfo = infoFactory.filterInfo(category);

	if (filterInfo != std::nullopt)
	{
		m_info->setFilterInfo(filterInfo.value());
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
		const bool hasSelection = !indexes.isEmpty();

		if (hasSelection)
		{
			const int row = index.row();

			const auto uniqueRowNumberPredicate = [row](const QModelIndex& modelIndex) 
			{ 
				return modelIndex.row() == row;
			};

			ASSERT(std::all_of(indexes.begin(), indexes.end(), uniqueRowNumberPredicate));

			const DCStorageDescription* storageDescription = m_summaryFilterModel->dataAccessor()->storageDescriptionByRow(row);
			actionRegistry.globalAction(exportFilterDataActionKey)->setData(QVariant::fromValue(*storageDescription));
		}

		actionRegistry.globalAction(exportFilterDataActionKey)->setEnabled(hasSelection);
	}
}

}
