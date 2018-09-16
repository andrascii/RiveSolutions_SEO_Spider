#include "website_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "istorage_adapter.h"
#include "storage_adapter_factory.h"
#include "page_data_widget.h"
#include "helpers.h"
#include "application.h"
#include "command_menu.h"
#include "model_helpers.h"
#include "page_data_widget_splitter.h"

namespace SeoSpider
{

WebSiteDataWidget::WebSiteDataWidget(PageDataWidget* pageDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_stackedWidget(new QStackedWidget(this))
	, m_pageDataWidget(pageDataWidget)
	, m_pageIndexBeforeShowNoResultsLabel(-1)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->setMargin(0);
	layout->setSpacing(0);

	QLabel* selectFilterLabel = new QLabel(this);
	selectFilterLabel->setObjectName(QStringLiteral("TablePlaseholderLabel"));
	selectFilterLabel->setText(tr("Select The Filter"));
	selectFilterLabel->setAlignment(Qt::AlignCenter);

	QLabel* noSearchResultsLabel = new QLabel(this);
	noSearchResultsLabel->setObjectName(QStringLiteral("NoSearchResultsLabel"));
	noSearchResultsLabel->setAlignment(Qt::AlignCenter);
	noSearchResultsLabel->setWordWrap(true);

	m_tables[StorageAdapterType::StorageAdapterTypeNone] = m_stackedWidget->addWidget(selectFilterLabel);
	m_tables[StorageAdapterType::StorageAdapterTypeNotFound] = m_stackedWidget->addWidget(noSearchResultsLabel);
	m_stackedWidget->setCurrentIndex(m_tables[StorageAdapterType::StorageAdapterTypeNone]);

	m_splitter = new PageDataWidgetSplitter(this, m_stackedWidget, m_pageDataWidget);
	layout->addWidget(m_splitter);
}

void WebSiteDataWidget::setStorageAdapterType(StorageAdapterType storageAdapterType, const QString& customDataFeed)
{
	auto tableIndexIterator = m_tables.find(storageAdapterType);

	if (tableIndexIterator != m_tables.end())
	{
		m_stackedWidget->setCurrentIndex(tableIndexIterator.value());
		return;
	}

	StorageAdapterFactory* factory = theApp->storageAdapterFactory();

	PageModel* pageModel = new PageModel(m_stackedWidget);
	IStorageAdapter* storageAdapter = factory->createParsedPageInfoStorage(storageAdapterType, theApp->sequencedDataCollection());

	if (IParsedPageStorageAdapter* pageAdapter = dynamic_cast<IParsedPageStorageAdapter*>(storageAdapter))
	{
		pageAdapter->setCustomDataFeed(customDataFeed);
	}

	m_storageAdapters[storageAdapterType] = storageAdapter;
	pageModel->setStorageAdapter(storageAdapter);

	TableView* tableView = new TableView(m_stackedWidget, false, true);
	PageViewModel* pageViewModel = new PageViewModel(tableView, pageModel, pageModel);

	tableView->setModel(pageModel);
	tableView->setViewModel(pageViewModel);
	tableView->setShowAdditionalGrid(true);
	tableView->setContextMenu(new CommandMenu(storageAdapter));

	m_tables[storageAdapterType] = m_stackedWidget->addWidget(tableView);
	m_stackedWidget->setCurrentIndex(m_tables[storageAdapterType]);

	VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(pageViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	if (m_pageDataWidget)
	{
		VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
			m_pageDataWidget, SLOT(pageViewSelectionChangedSlot(const QItemSelection&, const QItemSelection&))));
	}
}

void WebSiteDataWidget::setPageDataWidget(PageDataWidget* dataWidget)
{
	if (!dataWidget || dataWidget == m_pageDataWidget)
	{
		return;
	}

	m_pageDataWidget = dataWidget;
	m_splitter->addPageDataWidget(m_pageDataWidget);
}

PageDataWidget* WebSiteDataWidget::pageDataWidget() const noexcept
{
	return m_pageDataWidget;
}

void WebSiteDataWidget::pageViewSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	if (!m_pageDataWidget)
	{
		return;
	}

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

void WebSiteDataWidget::selectParsedPage(int row) const noexcept
{
	TableView* currentTableView = dynamic_cast<TableView*>(m_stackedWidget->currentWidget());

	if(!currentTableView)
	{
		ERRLOG << "Cannot cast to TableView";
		return;
	}

	const QModelIndex selectRowIndex = currentTableView->model()->index(row, 0);

	currentTableView->selectionModel()->select(selectRowIndex,
		QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	const int scrollTo = currentTableView->rowViewportPosition(row);
	currentTableView->scrollTo(selectRowIndex, QAbstractItemView::EnsureVisible);
}

QAbstractItemModel* WebSiteDataWidget::modelFor(StorageAdapterType storageAdapterType) const
{
	auto tableIndexIterator = m_tables.find(storageAdapterType);

	if (tableIndexIterator != m_tables.end())
	{
		TableView* tableView = qobject_cast<TableView*>(m_stackedWidget->widget(tableIndexIterator.value()));

		ASSERT(tableView);

		return tableView->model();
	}

	return nullptr;
}

void WebSiteDataWidget::showNoResultsLabelFor(const QString& searchValue)
{
	if (m_stackedWidget->currentIndex() != m_tables[StorageAdapterType::StorageAdapterTypeNotFound])
	{
		m_pageIndexBeforeShowNoResultsLabel = m_stackedWidget->currentIndex();
	}

	QLabel* noResultsLabel =
		qobject_cast<QLabel*>(m_stackedWidget->widget(m_tables[StorageAdapterType::StorageAdapterTypeNotFound]));

	ASSERT(noResultsLabel);

	noResultsLabel->setText(searchValue);
	m_stackedWidget->setCurrentIndex(m_tables[StorageAdapterType::StorageAdapterTypeNotFound]);
}

void WebSiteDataWidget::hideNoResultsLabel()
{
	m_stackedWidget->setCurrentIndex(m_pageIndexBeforeShowNoResultsLabel);
}

}
