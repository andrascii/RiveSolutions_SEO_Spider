#include "summary_filter_widget.h"
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

namespace QuickieWebBot
{

SummaryFilterWidget::SummaryFilterWidget(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui::SummaryFilterWidget)
{
	init();

	VERIFY(connect(m_ui->summaryTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void SummaryFilterWidget::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType)
{
	WebCrawler::DataCollection* dataCollection = theApp->modelController()->data();

	SummaryModel* summaryModel = 
		QuickieWebBotHelpers::safe_runtime_static_cast<SummaryModel*>(m_ui->summaryTableView->model());

	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, dataCollection);

	summaryModel->setDataAccessor(summaryDataAccessor);

	m_ui->summaryTableView->initSpan();
}

void SummaryFilterWidget::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (!selected.indexes().size())
	{
		return;
	}

	QModelIndex index = selected.indexes()[0];
	const SummaryModel* summaryModel = QuickieWebBotHelpers::safe_runtime_static_cast<const SummaryModel*>(index.model());

	SummaryCategoryItem category = summaryModel->itemCategory(index);

	PageInfoStorageModel* storageModel = 
		QuickieWebBotHelpers::safe_runtime_static_cast<PageInfoStorageModel*>(m_ui->summaryDetailsTableView->model());

	storageModel->setStorageAdaptor(theApp->storageAdaptorFactory()->create(category));

	m_ui->summaryDetailsTableView->viewModel()->invalidateRenderersCache();
}

void SummaryFilterWidget::init()
{
	m_ui->setupUi(this);

	initSummaryView();
	initDetailsView();
}

void SummaryFilterWidget::initSummaryView()
{
	m_ui->summaryTableView->resize(QuickieWebBotHelpers::pointsToPixels(100), m_ui->summaryTableView->height());

	SummaryModel* summaryModel = new SummaryModel(this);
	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);

	m_ui->summaryTableView->setModel(summaryModel);
	m_ui->summaryTableView->setViewModel(summaryViewModel);

	const int summaryViewWidth = QuickieWebBotHelpers::pointsToPixels(150);

	m_ui->splitter->setSizes(QList<int>() << summaryViewWidth << width() - summaryViewWidth);
}

void SummaryFilterWidget::initDetailsView()
{
	PageInfoStorageModel* model = new PageInfoStorageModel(this);
	PageInfoStorageViewModel* viewModel = new PageInfoStorageViewModel(model, this);

	model->setStorageAdaptor(theApp->storageAdaptorFactory()->create(SummaryCategoryItem::SummaryCategoryItemAllPages));

	m_ui->summaryDetailsTableView->setModel(model);
	m_ui->summaryDetailsTableView->setViewModel(viewModel);
}

}