#include "all_resources_widget.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "summary_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "web_site_pages_storage_model.h"
#include "web_site_pages_storage_view_model.h"


namespace QuickieWebBot
{

AllResourcesWidget::AllResourcesWidget(QWidget* parent)
	: QFrame(parent)
	, m_resourcesTableView(new TableView(this))
	, m_webResourcePagesTable(new TableView(this))
	, m_linksFromThisPage(new TableView(this))
	, m_linksToThisPage(new TableView(this))
	, m_httpResponse(new QPlainTextEdit(this))
{
	initializeResourcesTableView();
	initializewebResourcePagesTable();

	QSplitter* mainSplitter = new QSplitter(this);
	mainSplitter->setOrientation(Qt::Horizontal);
	mainSplitter->setOpaqueResize(false);
	mainSplitter->setChildrenCollapsible(false);

	QFrame* tablesFrame = new QFrame(this);

	QVBoxLayout* tablesLayout = new QVBoxLayout(tablesFrame);
	tablesLayout->setMargin(0);

	QTabWidget* tabWidget = new QTabWidget(tablesFrame);

	QSplitter* tablesSplitter = new QSplitter(this);
	tablesSplitter->setOrientation(Qt::Vertical);
	tablesSplitter->setOpaqueResize(false);
	tablesSplitter->setChildrenCollapsible(false);

	tabWidget->addTab(m_linksFromThisPage, tr("Links From This Page"));
	tabWidget->addTab(m_linksToThisPage, tr("Links To This Page"));
	tabWidget->addTab(m_httpResponse, tr("HTTP Response"));

	tablesSplitter->addWidget(m_webResourcePagesTable);
	tablesSplitter->addWidget(tabWidget);
	tablesLayout->addWidget(tablesSplitter);

	mainSplitter->addWidget(m_resourcesTableView);
	mainSplitter->addWidget(tablesFrame);

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(mainSplitter);

	VERIFY(connect(m_resourcesTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onFilterViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	const int summaryViewWidth = QuickieWebBotHelpers::pointsToPixels(50);
	mainSplitter->setSizes(QList<int>() << summaryViewWidth << width() - summaryViewWidth);
}

void AllResourcesWidget::initializeResourcesTableView()
{
	m_resourcesTableView->horizontalHeader()->setVisible(false);
	m_resourcesTableView->setSelectionMode(QAbstractItemView::SingleSelection);

	SummaryDataAccessorFactory::DataAccessorType dataAccessorType = SummaryDataAccessorFactory::DataAccessorType::AllResourcesPage;

	SummaryModel* summaryModel = new SummaryModel(this);
	summaryModel->setDataAccessor(theApp->summaryDataAccessorFactory()->create(dataAccessorType, theApp->guiStorage()));

	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);

	m_resourcesTableView->setModel(summaryModel);
	m_resourcesTableView->setViewModel(summaryViewModel);
}

void AllResourcesWidget::initializewebResourcePagesTable()
{
	WebSitePagesStorageModel* pagesModel = new WebSitePagesStorageModel(this);
	WebSitePagesStorageViewModel* pagesViewModel = new WebSitePagesStorageViewModel(pagesModel, this);

	m_webResourcePagesTable->setModel(pagesModel);
	m_webResourcePagesTable->setViewModel(pagesViewModel);


	WebSitePagesStorageModel* linksFromThisPageModel = new WebSitePagesStorageModel(this);
	WebSitePagesStorageViewModel* linksFromThisPageViewModel = new WebSitePagesStorageViewModel(linksFromThisPageModel, this);

	m_linksFromThisPage->setModel(linksFromThisPageModel);
	m_linksFromThisPage->setViewModel(linksFromThisPageViewModel);
}

void AllResourcesWidget::onFilterViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!selected.indexes().size())
	{
		return;
	}

	QModelIndex index = selected.indexes()[0];
	const SummaryModel* summaryModel = QuickieWebBotHelpers::safe_runtime_static_cast<const SummaryModel*>(index.model());

	StorageAdaptorType category = summaryModel->storageAdaptorType(index);

	WebSitePagesStorageModel* storageModel = dynamic_cast<WebSitePagesStorageModel*>(m_webResourcePagesTable->model());

	if (!storageModel)
	{
		ERRORLOG << "View does not have a model";
		return;
	}

	storageModel->setStorageAdaptor(theApp->storageAdaptorFactory()->createPageRawInfoStorage(category, theApp->guiStorage()));

	m_resourcesTableView->viewModel()->invalidateRenderersCache();

	if (WebSitePagesStorageModel* linksFromThisPageModel = 
		dynamic_cast<WebSitePagesStorageModel*>(m_linksFromThisPage->model()); linksFromThisPageModel)
	{
		StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();
		IStorageAdaptor* storageAdaptor = storageModel->storageAdaptor();

		linksFromThisPageModel->setStorageAdaptor(factory->createPageLinksStorage(PageLinkType::LinkFromThisPageType, storageAdaptor->pageRawInfoPtr(index)));
	}

}

}