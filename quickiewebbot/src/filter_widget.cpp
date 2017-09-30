#include "filter_widget.h"
#include "table_view.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "website_data_widget.h"
#include "quickie_web_bot_helpers.h"


namespace QuickieWebBot
{

FilterWidget::FilterWidget(WebSiteDataWidget* webSiteDataWidget, QWidget* parent)
	: QFrame(parent)
	, m_webSiteDataWidget(webSiteDataWidget)
	, m_summaryFilterTableView(new TableView(this))
	, m_summaryFilterModel(new SummaryModel(this))
	, m_summaryFilterViewModel(new SummaryViewModel(m_summaryFilterModel, this))
	, m_splitter(new QSplitter(this))
{
	m_summaryFilterTableView->setModel(m_summaryFilterModel);
	m_summaryFilterTableView->setViewModel(m_summaryFilterViewModel);
	m_summaryFilterTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_summaryFilterTableView->horizontalHeader()->hide();

	m_splitter->setOrientation(Qt::Horizontal);
	m_splitter->setChildrenCollapsible(false);
	m_splitter->addWidget(m_summaryFilterTableView);
	m_splitter->addWidget(m_webSiteDataWidget);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_splitter);

	VERIFY(connect(m_summaryFilterTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onSummaryViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

void FilterWidget::setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType)
{
	WebCrawler::SequencedDataCollection* guiStorage = theApp->sequencedDataCollection();
	ISummaryDataAccessor* summaryDataAccessor = theApp->summaryDataAccessorFactory()->create(dataAccessorType, guiStorage);
	m_summaryFilterModel->setDataAccessor(summaryDataAccessor);

	m_summaryFilterTableView->initSpans();
}

void FilterWidget::showEvent(QShowEvent* event)
{
	static bool isFirstShow = true;

	if (!isFirstShow)
	{
		return;
	}

	const int summaryViewWidth = QuickieWebBotHelpers::pointsToPixels(150);
	m_splitter->setSizes(QList<int>() << summaryViewWidth << width() - summaryViewWidth);

	event->ignore();

	isFirstShow = false;
}

void FilterWidget::onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	const QModelIndex index = selected.size() ? selected.indexes()[0] : QModelIndex();
	StorageAdaptorType category = m_summaryFilterModel->storageAdaptorType(index);

	m_webSiteDataWidget->setStorageAdaptorType(category);
}

}