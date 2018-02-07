#include "filter_widget.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "website_data_widget.h"
#include "helpers.h"
#include "application.h"
#include "command_menu.h"
#include "page_data_widget.h"

namespace SeoSpider
{

FilterWidget::FilterWidget(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_webSiteDataWidget(webSiteDataWidget)
	, m_summaryFilterTableView(new TableView(this))
	, m_summaryFilterModel(new SummaryModel(this))
	, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
	, m_isFirstShow(true)
{
	m_summaryFilterTableView->setModel(m_summaryFilterModel);
	m_summaryFilterTableView->setViewModel(m_summaryFilterViewModel);
	m_summaryFilterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryFilterTableView->horizontalHeader()->hide();
	m_summaryFilterTableView->setObjectName("FilterWidgetTableView");

	m_splitter->setOrientation(Qt::Horizontal);
	m_splitter->setChildrenCollapsible(false);
	m_splitter->addWidget(m_summaryFilterTableView);
	m_splitter->addWidget(m_webSiteDataWidget);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(m_splitter);

	VERIFY(connect(m_summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(theApp, &Application::mainWindowShown, this, &FilterWidget::adjustSize));
}

void FilterWidget::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType) const
{
	CrawlerEngine::SequencedDataCollection* guiStorage = theApp->sequencedDataCollection();
	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, guiStorage);
	m_summaryFilterModel->setDataAccessor(summaryDataAccessor);
	m_summaryFilterTableView->initSpans();
	m_summaryFilterTableView->setContextMenu(new CommandMenu(summaryDataAccessor));
}

void FilterWidget::selectFilter(CrawlerEngine::StorageType type)
{
	const int row = m_summaryFilterModel->dataAccessor()->rowByStorageType(type);
	m_summaryFilterTableView->selectionModel()->select(m_summaryFilterModel->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
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
	const StorageAdapterType category = m_summaryFilterModel->storageAdapterType(index);
	m_webSiteDataWidget->setStorageAdapterType(category);
}

}