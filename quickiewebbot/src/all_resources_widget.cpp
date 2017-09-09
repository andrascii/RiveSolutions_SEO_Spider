#include "all_resources_widget.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "summary_data_accessor_factory.h"
#include "quickie_web_bot_helpers.h"
#include "web_site_pages_storage_model.h"

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

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	QVBoxLayout* tablesLayout = new QVBoxLayout(this);



	QTabWidget* tabWidget = new QTabWidget(this);

	tabWidget->addTab(m_linksFromThisPage, tr("Links From This Page"));
	tabWidget->addTab(m_linksToThisPage, tr("Links To This Page"));
	tabWidget->addTab(m_httpResponse, tr("HTTP Response"));

	tablesLayout->addWidget(m_webResourcePagesTable);
	tablesLayout->addWidget(tabWidget);

	mainLayout->addWidget(m_resourcesTableView);
	mainLayout->addLayout(tablesLayout);

	VERIFY(connect(m_resourcesTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onFilterViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void AllResourcesWidget::initializeResourcesTableView()
{
	SummaryDataAccessorFactory::DataAccessorType dataAccessorType = SummaryDataAccessorFactory::DataAccessorType::AllResourcesPage;

	SummaryModel* summaryModel = new SummaryModel(this);
	summaryModel->setDataAccessor(theApp->summaryDataAccessorFactory()->create(dataAccessorType, theApp->guiStorage()));

	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);

	m_resourcesTableView->setModel(summaryModel);
	m_resourcesTableView->setViewModel(summaryViewModel);
}

void AllResourcesWidget::onFilterViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!selected.indexes().size())
	{
		return;
	}

	QModelIndex index = selected.indexes()[0];
	const SummaryModel* summaryModel = QuickieWebBotHelpers::safe_runtime_static_cast<const SummaryModel*>(index.model());

	SummaryCategoryItem category = summaryModel->itemCategory(index);

	WebSitePagesStorageModel* storageModel =
		QuickieWebBotHelpers::safe_runtime_static_cast<WebSitePagesStorageModel*>(m_resourcesTableView->model());

	storageModel->setStorageAdaptor(theApp->storageAdaptorFactory()->create(category));

	m_resourcesTableView->viewModel()->invalidateRenderersCache();
}

}