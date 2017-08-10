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

namespace QuickieWebBot
{

ErrorsFilterWidget::ErrorsFilterWidget(QWidget* parent)
	: QWidget(parent)
	, m_summaryGridView(nullptr)
	, m_summaryDetailsGridView(nullptr)
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(this);
	verticalLayout->setContentsMargins(9, -1, -1, -1);

	QSplitter* splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Horizontal);

	m_summaryGridView = new GridView(splitter);
	m_summaryGridView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryGridView->setShowGrid(false);
	m_summaryGridView->setGridStyle(Qt::NoPen);
	m_summaryGridView->horizontalHeader()->setVisible(false);
	m_summaryGridView->verticalHeader()->setVisible(false);

	SummaryModel* summaryModel = new SummaryModel(this);
	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);
	m_summaryGridView->setModel(summaryModel);
	m_summaryGridView->setViewModel(summaryViewModel);

	m_summaryDetailsGridView = new GridView(splitter);

	PageInfoStorageModel* model = new PageInfoStorageModel(this);
	PageInfoStorageViewModel* viewModel = new PageInfoStorageViewModel(model, this);
	
	StorageAdaptorFactory* storageAdaptorFactory = theApp->modelController()->data()->storageAdaptorFactory();
	model->setStorageAdaptor(storageAdaptorFactory->create(DataCollection::CrawledUrlStorageType));

	m_summaryDetailsGridView->setModel(model);
	m_summaryDetailsGridView->setViewModel(viewModel);

	splitter->addWidget(m_summaryGridView);
	splitter->addWidget(m_summaryDetailsGridView);
	verticalLayout->addWidget(splitter);

	setLayout(verticalLayout);

 	VERIFY(connect(m_summaryGridView, SIGNAL(selectionWasChanged(const QItemSelection&, const QItemSelection&)),
 		this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))));
}

void ErrorsFilterWidget::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	assert(selected.indexes().size() > 1);
	QModelIndex index = selected.indexes()[0];

	StorageAdaptorFactory* storageAdaptorFactory = theApp->modelController()->data()->storageAdaptorFactory();

	DataCollection::StorageType storageType = 
		static_cast<DataCollection::StorageType>(index.data(IGridModel::WhatsThisRole).toInt());

	PageInfoStorageModel* storageModel = dynamic_cast<PageInfoStorageModel*>(m_summaryDetailsGridView->model());
	storageModel->setStorageAdaptor(storageAdaptorFactory->create(storageType));
}

}