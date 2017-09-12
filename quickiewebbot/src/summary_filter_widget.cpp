#include "summary_filter_widget.h"
#include "application.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "page_model.h"
#include "page_view_model.h"
#include "data_collection.h"
#include "page_raw_info_storage_adaptor.h"
#include "storage_adaptor_factory.h"
#include "model_controller.h"
#include "quickie_web_bot_helpers.h"
#include "storage_adaptor_type.h"

namespace QuickieWebBot
{

SummaryFilterWidget::SummaryFilterWidget(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui::SummaryFilterWidget)
{
	m_ui->setupUi(this);

	initSummaryView();
	initDetailsView();

	VERIFY(connect(m_ui->summaryTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void SummaryFilterWidget::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType)
{
	WebCrawler::GuiStorage* guiStorage = theApp->guiStorage();

	SummaryModel* summaryModel = 
		QuickieWebBotHelpers::safe_runtime_static_cast<SummaryModel*>(m_ui->summaryTableView->model());

	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, guiStorage);

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

	StorageAdaptorType category = summaryModel->storageAdaptorType(index);

	PageModel* storageModel = dynamic_cast<PageModel*>(m_ui->summaryDetailsTableView->model());

	if (!storageModel)
	{
		ERRORLOG << "View does not have a model";
		return;
	}

	storageModel->setStorageAdaptor(theApp->storageAdaptorFactory()->createPageRawInfoStorage(category, theApp->guiStorage()));

	m_ui->summaryDetailsTableView->viewModel()->invalidateItemViewRendererCache();
}

void SummaryFilterWidget::initSummaryView()
{
	SummaryModel* summaryModel = new SummaryModel(this);
	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);

	m_ui->summaryTableView->setModel(summaryModel);
	m_ui->summaryTableView->setViewModel(summaryViewModel);

	const int summaryViewWidth = QuickieWebBotHelpers::pointsToPixels(120);

	m_ui->splitter->setSizes(QList<int>() << summaryViewWidth << width() - summaryViewWidth);
}

void SummaryFilterWidget::initDetailsView()
{
	PageModel* model = new PageModel(this);
	PageViewModel* viewModel = new PageViewModel(model, this);

	model->setStorageAdaptor(theApp->storageAdaptorFactory()->createPageRawInfoStorage(StorageAdaptorType::StorageAdaptorTypeAllPages, theApp->guiStorage()));

	m_ui->summaryDetailsTableView->setModel(model);
	m_ui->summaryDetailsTableView->setViewModel(viewModel);
}

}