#include "errors_filter_widget.h"
#include "application.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "data_collection.h"
#include "storage_adaptor.h"
#include "storage_adaptor_factory.h"
#include "model_controller.h"
#include "quickie_web_bot_helpers.h"
#include "summary_category_item.h"
#include "summary_data_accessor_factory.h"

namespace QuickieWebBot
{

ErrorsFilterWidget::ErrorsFilterWidget(QWidget* parent)
	: QWidget(parent)
	, m_summaryTableView(nullptr)
	, m_summaryDetailsTableView(nullptr)
{
	init();

	VERIFY(connect(m_summaryTableView, SIGNAL(selectionWasChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void ErrorsFilterWidget::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!selected.indexes().size())
	{
		return;
	}

	QModelIndex index = selected.indexes()[0];
	const SummaryModel* summaryModel = QuickieWebBotHelpers::safe_runtime_static_cast<const SummaryModel*>(index.model());

	SummaryCategoryItem category = summaryModel->itemCategory(index);

	PageInfoStorageModel* storageModel = 
		QuickieWebBotHelpers::safe_runtime_static_cast<PageInfoStorageModel*>(m_summaryDetailsTableView->model());

	storageModel->setStorageAdaptor(theApp->storageAdapterFactory()->create(category));

	m_summaryDetailsTableView->viewModel()->resetRenderersCache();
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

	m_summaryTableView->setParent(splitter);
	m_summaryDetailsTableView->setParent(splitter);

	splitter->addWidget(m_summaryTableView);
	splitter->addWidget(m_summaryDetailsTableView);
	verticalLayout->addWidget(splitter);

	setLayout(verticalLayout);
}

void ErrorsFilterWidget::initSummaryView()
{
	m_summaryTableView = new TableView;
	m_summaryTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryTableView->setShowGrid(false);
	m_summaryTableView->setGridStyle(Qt::NoPen);
	m_summaryTableView->horizontalHeader()->setVisible(false);
	m_summaryTableView->verticalHeader()->setVisible(false);
	m_summaryTableView->resize(QuickieWebBotHelpers::pointsToPixels(100), m_summaryTableView->height());

	SummaryModel* summaryModel = new SummaryModel(this);
	summaryModel->setDataAccessor(theApp->summaryDataAccessorFactory()->create(SummaryDataAccessorFactory::DataAccessorType::ErrorsFilterPage));

	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);

	m_summaryTableView->setModel(summaryModel);
	m_summaryTableView->setViewModel(summaryViewModel);
}

void ErrorsFilterWidget::initDetailsView()
{
	m_summaryDetailsTableView = new TableView;

	PageInfoStorageModel* model = new PageInfoStorageModel(this);
	PageInfoStorageViewModel* viewModel = new PageInfoStorageViewModel(model, this);

	model->setStorageAdaptor(theApp->storageAdapterFactory()->create(SummaryCategoryItem::SummaryCategoryItemAllPages));

	m_summaryDetailsTableView->setModel(model);
	m_summaryDetailsTableView->setViewModel(viewModel);
}

}