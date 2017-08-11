#include "errors_filter_widget.h"
#include "application.h"
#include "grid_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "data_collection.h"
#include "storage_adaptor.h"
#include "storage_adaptor_factory.h"
#include "model_controller.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

ErrorsFilterWidget::ErrorsFilterWidget(QWidget* parent)
	: QWidget(parent)
	, m_summaryGridView(nullptr)
	, m_summaryDetailsGridView(nullptr)
{
	init();

	VERIFY(connect(m_summaryGridView, SIGNAL(selectionWasChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void ErrorsFilterWidget::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	assert(selected.indexes().size() > 1);
	QModelIndex index = selected.indexes()[0];

	StorageAdaptorFactory* storageAdaptorFactory = theApp->modelController()->data()->storageAdaptorFactory();

	DataCollection::StorageType storageType = 
		static_cast<DataCollection::StorageType>(index.data(IGridModel::WhatsThisRole).toInt());

	PageInfoStorageModel* storageModel = dynamic_cast<PageInfoStorageModel*>(m_summaryDetailsGridView->model());
	storageModel->setStorageAdaptor(storageAdaptorFactory->create(storageType));

	m_summaryDetailsGridView->viewModel()->resetRenderersCache();
}

void ErrorsFilterWidget::init()
{
	initSummaryView();
	initDetailsView();

	QVBoxLayout* verticalLayout = new QVBoxLayout(this);
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	QSplitter* splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Horizontal);
	splitter->setOpaqueResize(false);

	m_summaryGridView->setParent(splitter);
	m_summaryDetailsGridView->setParent(splitter);

	splitter->addWidget(m_summaryGridView);
	splitter->addWidget(m_summaryDetailsGridView);
	verticalLayout->addWidget(splitter);

	setLayout(verticalLayout);
}

void ErrorsFilterWidget::initSummaryView()
{
	m_summaryGridView = new GridView;
	m_summaryGridView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryGridView->setShowGrid(false);
	m_summaryGridView->setGridStyle(Qt::NoPen);
	m_summaryGridView->horizontalHeader()->setVisible(false);
	m_summaryGridView->verticalHeader()->setVisible(false);
	m_summaryGridView->resize(QuickieWebBotHelpers::pointsToPixels(20), m_summaryGridView->height());

	SummaryModel* summaryModel = new SummaryModel(this);
	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);
	m_summaryGridView->setModel(summaryModel);
	m_summaryGridView->setViewModel(summaryViewModel);
}

void ErrorsFilterWidget::initDetailsView()
{
	m_summaryDetailsGridView = new GridView;

	PageInfoStorageModel* model = new PageInfoStorageModel(this);
	PageInfoStorageViewModel* viewModel = new PageInfoStorageViewModel(model, this);

	StorageAdaptorFactory* storageAdaptorFactory = theApp->modelController()->data()->storageAdaptorFactory();
	model->setStorageAdaptor(storageAdaptorFactory->create(DataCollection::CrawledUrlStorageType));

	m_summaryDetailsGridView->setModel(model);
	m_summaryDetailsGridView->setViewModel(viewModel);
}

}