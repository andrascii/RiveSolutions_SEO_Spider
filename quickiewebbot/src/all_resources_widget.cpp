#include "all_resources_widget.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "summary_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "page_model.h"
#include "page_view_model.h"


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

	m_mainSplitter = new QSplitter(this);
	m_mainSplitter->setOrientation(Qt::Horizontal);
	m_mainSplitter->setOpaqueResize(false);
	m_mainSplitter->setChildrenCollapsible(false);

	QFrame* tablesFrame = new QFrame(this);

	QVBoxLayout* tablesLayout = new QVBoxLayout(tablesFrame);
	tablesLayout->setMargin(0);

	QTabWidget* tabWidget = new QTabWidget(tablesFrame);

	m_tablesSplitter = new QSplitter(tablesFrame);
	m_tablesSplitter->setOrientation(Qt::Vertical);
	m_tablesSplitter->setOpaqueResize(false);
	m_tablesSplitter->setChildrenCollapsible(false);

	tabWidget->addTab(m_linksFromThisPage, tr("Links On This Page"));
	tabWidget->addTab(m_linksToThisPage, tr("Links To This Page"));
	tabWidget->addTab(m_httpResponse, tr("HTTP Response"));

	m_tablesSplitter->addWidget(m_webResourcePagesTable);
	m_tablesSplitter->addWidget(tabWidget);
	tablesLayout->addWidget(m_tablesSplitter);

	m_mainSplitter->addWidget(m_resourcesTableView);
	m_mainSplitter->addWidget(tablesFrame);

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_mainSplitter);

	setLayout(mainLayout);

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
	PageModel* pagesModel = new PageModel(this);
	PageViewModel* pagesViewModel = new PageViewModel(pagesModel, this);

	m_webResourcePagesTable->setModel(pagesModel);
	m_webResourcePagesTable->setViewModel(pagesViewModel);

	// links from this page
	PageModel* linksFromThisPageModel = new PageModel(this);
	PageViewModel* linksFromThisPageViewModel = new PageViewModel(linksFromThisPageModel, this);

	m_linksFromThisPage->setModel(linksFromThisPageModel);
	m_linksFromThisPage->setViewModel(linksFromThisPageViewModel);

	// links to this page
	PageModel* linksToThisPageModel = new PageModel(this);
	PageViewModel* linksToThisPageViewModel = new PageViewModel(linksToThisPageModel, this);

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

	PageModel* storageModel = dynamic_cast<PageModel*>(m_webResourcePagesTable->model());

	if (!storageModel)
	{
		ERRORLOG << "m_webResourcePagesTable does not have a model";
		return;
	}

	storageModel->setStorageAdaptor(theApp->storageAdaptorFactory()->createPageRawInfoStorage(category, theApp->guiStorage()));

	m_resourcesTableView->viewModel()->invalidateItemViewRendererCache();
}

void AllResourcesWidget::onPageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!selected.indexes().size())
	{
		return;
	}

	QModelIndex index = selected.indexes()[0];

	PageModel* storageModel = dynamic_cast<PageModel*>(m_webResourcePagesTable->model());

	if (!storageModel)
	{
		ERRORLOG << "m_webResourcePagesTable does not have a model";
		return;
	}

	StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();
	IStorageAdaptor* storageAdaptor = storageModel->storageAdaptor();

	if (PageModel* linksFromThisPageModel =
		dynamic_cast<PageModel*>(m_linksFromThisPage->model()); linksFromThisPageModel)
	{
		IStorageAdaptor* newPageInfoAdaptor = factory->createPageLinksStorage(PageLinkType::LinkFromThisPageType, storageAdaptor->pageRawInfoPtr(index));
		linksFromThisPageModel->setStorageAdaptor(newPageInfoAdaptor);
	}

	if (PageModel* linksToThisPageModel =
		dynamic_cast<PageModel*>(m_linksToThisPage->model()); linksToThisPageModel)
	{
		StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();
		IStorageAdaptor* storageAdaptor = storageModel->storageAdaptor();

		linksToThisPageModel->setStorageAdaptor(factory->createPageLinksStorage(PageLinkType::LinkToThisPageType, storageAdaptor->pageRawInfoPtr(index)));
	}

	setPageServerResponse(storageAdaptor->pageRawInfoPtr(index));
}

void AllResourcesWidget::setPageServerResponse(const PageRawInfoPtr& page)
{
	m_httpResponse->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QString selectedPageServerResponse = page->itemValue(PageRawInfo::ServerResponseItemType).toString().trimmed();

	if (selectedPageServerResponse.isEmpty())
	{
		QPixmap oopsPixmap(":/images/oops.jpg");

		m_httpResponse->setPixmap(oopsPixmap.scaled(QSize(oopsPixmap.width() / 3, oopsPixmap.height() / 3)));
		return;
	}

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

	m_httpResponse->setText(selectedPageServerResponse);
}

void AllResourcesWidget::showEvent(QShowEvent* event)
{
	const int summaryViewWidth = QuickieWebBotHelpers::pointsToPixels(230);
	m_mainSplitter->setSizes(QList<int>() << summaryViewWidth << width() - summaryViewWidth);

	const int webResourcePagesTableHeight = QuickieWebBotHelpers::pointsToPixels(300);
	m_tablesSplitter->setSizes(QList<int>() << webResourcePagesTableHeight << height() - webResourcePagesTableHeight);

	event->ignore();
}

}