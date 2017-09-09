#include "all_resources_widget.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "summary_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "web_site_pages_model.h"
#include "web_site_pages_view_model.h"


namespace QuickieWebBot
{

AllResourcesWidget::AllResourcesWidget(QWidget* parent)
	: QFrame(parent)
	, m_resourcesTableView(new TableView(this))
	, m_webResourcePagesTable(new TableView(this))
	, m_linksFromThisPage(new TableView(this))
	, m_linksToThisPage(new TableView(this))
	, m_httpResponse(new QLabel(this))
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

	setLayout(mainLayout);

	const int summaryViewWidth = QuickieWebBotHelpers::pointsToPixels(20);
	mainSplitter->setSizes(QList<int>() << summaryViewWidth << width() - summaryViewWidth);

	VERIFY(connect(m_resourcesTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onFilterViewSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(m_webResourcePagesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onPageViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
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
	WebSitePagesModel* pagesModel = new WebSitePagesModel(this);
	WebSitePagesViewModel* pagesViewModel = new WebSitePagesViewModel(pagesModel, this);

	m_webResourcePagesTable->setModel(pagesModel);
	m_webResourcePagesTable->setViewModel(pagesViewModel);

	// links from this page
	WebSitePagesModel* linksFromThisPageModel = new WebSitePagesModel(this);
	WebSitePagesViewModel* linksFromThisPageViewModel = new WebSitePagesViewModel(linksFromThisPageModel, this);

	m_linksFromThisPage->setModel(linksFromThisPageModel);
	m_linksFromThisPage->setViewModel(linksFromThisPageViewModel);

	// links to this page
	WebSitePagesModel* linksToThisPageModel = new WebSitePagesModel(this);
	WebSitePagesViewModel* linksToThisPageViewModel = new WebSitePagesViewModel(linksToThisPageModel, this);

	m_linksToThisPage->setModel(linksToThisPageModel);
	m_linksToThisPage->setViewModel(linksToThisPageViewModel);
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

	WebSitePagesModel* storageModel = dynamic_cast<WebSitePagesModel*>(m_webResourcePagesTable->model());

	if (!storageModel)
	{
		ERRORLOG << "m_webResourcePagesTable does not have a model";
		return;
	}

	storageModel->setStorageAdaptor(theApp->storageAdaptorFactory()->createPageRawInfoStorage(category, theApp->guiStorage()));

	m_resourcesTableView->viewModel()->invalidateRenderersCache();
}

void AllResourcesWidget::onPageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!selected.indexes().size())
	{
		return;
	}

	QModelIndex index = selected.indexes()[0];

	WebSitePagesModel* storageModel = dynamic_cast<WebSitePagesModel*>(m_webResourcePagesTable->model());

	if (!storageModel)
	{
		ERRORLOG << "m_webResourcePagesTable does not have a model";
		return;
	}

	StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();
	IStorageAdaptor* storageAdaptor = storageModel->storageAdaptor();

	if (WebSitePagesModel* linksFromThisPageModel =
		dynamic_cast<WebSitePagesModel*>(m_linksFromThisPage->model()); linksFromThisPageModel)
	{
		IStorageAdaptor* newPageInfoAdaptor = factory->createPageLinksStorage(PageLinkType::LinkFromThisPageType, storageAdaptor->pageRawInfoPtr(index));
		linksFromThisPageModel->setStorageAdaptor(newPageInfoAdaptor);
	}

	if (WebSitePagesModel* linksToThisPageModel =
		dynamic_cast<WebSitePagesModel*>(m_linksToThisPage->model()); linksToThisPageModel)
	{
		StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();
		IStorageAdaptor* storageAdaptor = storageModel->storageAdaptor();

		linksToThisPageModel->setStorageAdaptor(factory->createPageLinksStorage(PageLinkType::LinkToThisPageType, storageAdaptor->pageRawInfoPtr(index)));
	}

	setPageServerResponse(storageAdaptor->pageRawInfoPtr(index));
}

void AllResourcesWidget::setPageServerResponse(const PageRawInfoPtr& page)
{
	QString selectedPageServerResponse = page->itemValue(PageRawInfo::ServerResponseItemType).toString();

	QStringList serverResponseHeaders = selectedPageServerResponse.split("\n");

	for (int i = 0; i < serverResponseHeaders.size(); ++i)
	{
		serverResponseHeaders[i].insert(0, "<b>");
		serverResponseHeaders[i].insert(serverResponseHeaders[i].indexOf(":") + 1, "</b>");
	}

	selectedPageServerResponse.clear();

	for (int i = 0; i < serverResponseHeaders.size(); ++i)
	{
		selectedPageServerResponse += serverResponseHeaders[i] + "<br>";
	}

	m_httpResponse->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_httpResponse->setText(selectedPageServerResponse);
}

}