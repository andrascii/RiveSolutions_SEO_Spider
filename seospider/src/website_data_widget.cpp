#include "website_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "istorage_adapter.h"
#include "storage_adapter_factory.h"
#include "page_data_widget.h"
#include "seo_spider_helpers.h"
#include "application.h"
#include "deferred_call.h"

namespace SeoSpider
{

WebSiteDataWidget::WebSiteDataWidget(PageDataWidget* pageDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_splitter(new QSplitter(this))
	, m_stackedWidget(new QStackedWidget(this))
	, m_pageDataWidget(pageDataWidget)
	, m_wasShown(false)
{
	m_splitter->setOrientation(Qt::Vertical);
	m_splitter->setChildrenCollapsible(false);

	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(m_splitter);

	layout->setMargin(0);
	layout->setSpacing(0);

	QLabel* selectFilterLabel = new QLabel(this);
	selectFilterLabel->setObjectName(QStringLiteral("SelectFilterLabel"));
	selectFilterLabel->setText(tr("Select The Filter"));
	selectFilterLabel->setAlignment(Qt::AlignCenter);

	m_tables[StorageAdapterType::StorageAdapterTypeNone] = m_stackedWidget->addWidget(selectFilterLabel);
	m_stackedWidget->setCurrentIndex(m_tables[StorageAdapterType::StorageAdapterTypeNone]);

	m_splitter->addWidget(m_stackedWidget);

	if (m_pageDataWidget)
	{
		m_splitter->addWidget(m_pageDataWidget);
	}
}

void WebSiteDataWidget::setStorageAdapterType(StorageAdapterType storageAdapterType)
{
	auto tableIndexIterator = m_tables.find(storageAdapterType);

	if (tableIndexIterator != m_tables.end())
	{
		m_stackedWidget->setCurrentIndex(tableIndexIterator.value());
		return;
	}

	StorageAdapterFactory* factory = theApp->storageAdapterFactory();

	PageModel* pageModel = new PageModel(m_stackedWidget);
	pageModel->setStorageAdapter(factory->createParsedPageInfoStorage(storageAdapterType, theApp->sequencedDataCollection()));


	TableView* tableView = new TableView(m_stackedWidget);
	PageViewModel* pageViewModel = new PageViewModel(tableView, pageModel, pageModel);

	tableView->setModel(pageModel);
	tableView->setViewModel(pageViewModel);
	tableView->setShowAdditionalGrid(true);

	m_tables[storageAdapterType] = m_stackedWidget->addWidget(tableView);
	m_stackedWidget->setCurrentIndex(m_tables[storageAdapterType]);

	VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(pageViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void WebSiteDataWidget::showEvent(QShowEvent*)
{
	if (m_wasShown)
	{
		return;
	}

	QWidget* parentWidget = qobject_cast<QWidget*>(parent());

	ASSERT(parentWidget);

	const int parentWidgetHeight = parentWidget->height();

	const int mainTableView = SeoSpiderHelpers::pointsToPixels(400);

	m_splitter->setSizes(QList<int>() << mainTableView << parentWidgetHeight - mainTableView);

	m_wasShown = true;
}

void WebSiteDataWidget::pageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!m_pageDataWidget)
	{
		return;
	}

	const QModelIndex index = selected.size() ? selected.indexes()[0] : QModelIndex();
	
	if (const PageModel* storageModel = dynamic_cast<const PageModel*>(index.model()); storageModel)
	{
		const IStorageAdapter* storageAdapter = storageModel->storageAdapter();
		m_pageDataWidget->setParsedPageInfo(storageAdapter->parsedPageInfoPtr(index));
	}
}

}