#include "website_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "istorage_adaptor.h"
#include "storage_adaptor_factory.h"
#include "page_data_widget.h"
#include "quickie_web_bot_helpers.h"
#include "application.h"


namespace QuickieWebBot
{

WebSiteDataWidget::WebSiteDataWidget(PageDataWidget* pageDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_splitter(new QSplitter(this))
	, m_stackedWidget(new QStackedWidget(this))
	, m_pageDataWidget(pageDataWidget)
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

	m_tables[StorageAdaptorType::StorageAdaptorTypeNone] = m_stackedWidget->addWidget(selectFilterLabel);
	m_stackedWidget->setCurrentIndex(m_tables[StorageAdaptorType::StorageAdaptorTypeNone]);

	m_splitter->addWidget(m_stackedWidget);

	if (m_pageDataWidget)
	{
		m_splitter->addWidget(m_pageDataWidget);
	}
}

void WebSiteDataWidget::setStorageAdaptorType(StorageAdaptorType storageAdaptorType)
{
	auto tableIndexIterator = m_tables.find(storageAdaptorType);

	if (tableIndexIterator != m_tables.end())
	{
		m_stackedWidget->setCurrentIndex(tableIndexIterator.value());
		return;
	}

	StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();

	PageModel* pageModel = new PageModel(m_stackedWidget);
	pageModel->setStorageAdaptor(factory->createPageRawInfoStorage(storageAdaptorType, theApp->sequencedDataCollection()));

	PageViewModel* pageViewModel = new PageViewModel(pageModel, pageModel);

	TableView* tableView = new TableView(m_stackedWidget);
	tableView->setModel(pageModel);
	tableView->setViewModel(pageViewModel);
	tableView->setShowAdditionalGrid(true);

	m_tables[storageAdaptorType] = m_stackedWidget->addWidget(tableView);
	m_stackedWidget->setCurrentIndex(m_tables[storageAdaptorType]);

	VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(pageViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
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
		const IStorageAdaptor* storageAdaptor = storageModel->storageAdaptor();
		m_pageDataWidget->setPageRawInfo(storageAdaptor->parsedPageInfoPtr(index));
	}
}

void WebSiteDataWidget::showEvent(QShowEvent* event)
{
	static bool isFirstShow = true;

	if (!isFirstShow)
	{
		return;
	}

	const int mainTableView = QuickieWebBotHelpers::pointsToPixels(700);
	m_splitter->setSizes(QList<int>() << mainTableView << height() - mainTableView);

	event->ignore();

	isFirstShow = false;
}

}