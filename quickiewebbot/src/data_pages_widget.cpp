#include "data_pages_widget.h"
#include "application.h"
#include "table_view.h"
#include "summary_filter_widget.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "storage_adaptor.h"
#include "storage_adaptor_factory.h"
#include "model_controller.h"
#include "context_menu_data_collection_row.h"
#include "data_collection.h"
#include "quickie_web_bot_helpers.h"
#include "titled_widget.h"

namespace QuickieWebBot
{

DataPagesWidget::DataPagesWidget(QWidget* parent)
	: QFrame(parent)
{
	initializeNavigationPanelWidget();
	initializeStackedWidget();

	QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->addWidget(m_navigationPanel.navigationPanelWidget);
	horizontalLayout->addWidget(m_stackedWidget);

	setLayout(horizontalLayout);
}

void DataPagesWidget::showPage(Page page)
{
	m_stackedWidget->setCurrentIndex(m_pageIndexes[page]);
}

void DataPagesWidget::handleNavigationPanelButtonClick()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	DEBUG_ASSERT(button);

	if (m_prevButton == button)
	{
		return;
	}

	if (button->property("subButton").toBool() == false &&
		button == m_navigationPanel.pushButtons[Page::SiteStructurePanelPage])
	{
		const bool isSiteStructurePanelButtonVisible = m_navigationPanel.siteStructurePanelWidget->isVisible();
		m_navigationPanel.siteStructurePanelWidget->setVisible(!isSiteStructurePanelButtonVisible);

		return;
	}

	if (m_prevButton)
	{
		m_prevButton->setProperty("selected", false);
		m_prevButton->repaint();
		m_prevButton->style()->unpolish(m_prevButton);
		m_prevButton->style()->polish(m_prevButton);
	}

	button->setProperty("selected", true);
	button->style()->unpolish(button);
	button->style()->polish(button);

	m_prevButton = button;

	auto pushButtonsIterator = std::find_if(
		std::begin(m_navigationPanel.pushButtons),
		std::end(m_navigationPanel.pushButtons), 
		[button](const auto& pair) { return pair.second == button; }
	);

	DEBUG_ASSERT(pushButtonsIterator != std::end(m_navigationPanel.pushButtons));

	showPage(pushButtonsIterator->first);
}

void DataPagesWidget::initializeNavigationPanelWidget()
{
	m_navigationPanel.navigationPanelWidget = new QWidget(this);
	m_navigationPanel.verticalMainLayout = new QVBoxLayout(m_navigationPanel.navigationPanelWidget);

	m_navigationPanel.siteStructurePanelWidget = new QWidget(m_navigationPanel.navigationPanelWidget);
	m_navigationPanel.verticalSubbuttonsLayout = new QVBoxLayout(m_navigationPanel.siteStructurePanelWidget);


	m_navigationPanel.pushButtons[Page::SiteStructurePanelPage] = 
		new QPushButton(QStringLiteral("Site Structure"), m_navigationPanel.navigationPanelWidget);

	m_navigationPanel.pushButtons[Page::SeoAnalysisPage] = 
		new QPushButton(QStringLiteral("Seo Analysis"), m_navigationPanel.navigationPanelWidget);

	m_navigationPanel.pushButtons[Page::AllPagesPage] = 
		new QPushButton(QStringLiteral("All Site Pages"), m_navigationPanel.navigationPanelWidget);

	m_navigationPanel.pushButtons[Page::AllResourcesPage] = 
		new QPushButton(QStringLiteral("All Resources"), m_navigationPanel.navigationPanelWidget);

	m_navigationPanel.pushButtons[Page::DomainMetricsPage] = 
		new QPushButton(QStringLiteral("Domain Metrics"), m_navigationPanel.navigationPanelWidget);

	m_navigationPanel.pushButtons[Page::ReportsPage] = 
		new QPushButton(QStringLiteral("Reports"), m_navigationPanel.navigationPanelWidget);


	m_navigationPanel.pushButtons[Page::SeoAnalysisPage]->setProperty("subButton", true);
	m_navigationPanel.pushButtons[Page::AllPagesPage]->setProperty("subButton", true);
	m_navigationPanel.pushButtons[Page::AllResourcesPage]->setProperty("subButton", true);
	m_navigationPanel.pushButtons[Page::SeoAnalysisPage]->setProperty("selected", true);
	m_prevButton = m_navigationPanel.pushButtons[Page::SeoAnalysisPage];


	VERIFY(connect(m_navigationPanel.pushButtons[Page::SiteStructurePanelPage], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));

	VERIFY(connect(m_navigationPanel.pushButtons[Page::SeoAnalysisPage], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));

	VERIFY(connect(m_navigationPanel.pushButtons[Page::AllPagesPage], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));

	VERIFY(connect(m_navigationPanel.pushButtons[Page::AllResourcesPage], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));

	VERIFY(connect(m_navigationPanel.pushButtons[Page::DomainMetricsPage], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));

	VERIFY(connect(m_navigationPanel.pushButtons[Page::ReportsPage], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));


	m_navigationPanel.verticalSubbuttonsLayout->addWidget(m_navigationPanel.pushButtons[Page::SeoAnalysisPage]);
	m_navigationPanel.verticalSubbuttonsLayout->addWidget(m_navigationPanel.pushButtons[Page::AllPagesPage]);
	m_navigationPanel.verticalSubbuttonsLayout->addWidget(m_navigationPanel.pushButtons[Page::AllResourcesPage]);
	m_navigationPanel.verticalSubbuttonsLayout->setSpacing(0);
	m_navigationPanel.verticalSubbuttonsLayout->setContentsMargins(-1, 0, 0, 0);
	m_navigationPanel.siteStructurePanelWidget->setLayout(m_navigationPanel.verticalSubbuttonsLayout);


	m_navigationPanel.verticalMainLayout->addWidget(m_navigationPanel.pushButtons[Page::SiteStructurePanelPage]);
	m_navigationPanel.verticalMainLayout->addWidget(m_navigationPanel.siteStructurePanelWidget);
	m_navigationPanel.verticalMainLayout->addWidget(m_navigationPanel.pushButtons[Page::DomainMetricsPage]);
	m_navigationPanel.verticalMainLayout->addWidget(m_navigationPanel.pushButtons[Page::ReportsPage]);
	m_navigationPanel.verticalMainLayout->setSpacing(0);

	m_navigationPanel.verticalMainLayout->addItem(
		new QSpacerItem(
			QuickieWebBotHelpers::pointsToPixels(15),
			QuickieWebBotHelpers::pointsToPixels(30), 
			QSizePolicy::Minimum, QSizePolicy::Expanding)
	);

	m_navigationPanel.navigationPanelWidget->setLayout(m_navigationPanel.verticalMainLayout);

	// use this name for customize style through stylesheets
	m_navigationPanel.navigationPanelWidget->setObjectName(QStringLiteral("NavigationPanel"));
}

void DataPagesWidget::initializeStackedWidget()
{
	m_stackedWidget = new QStackedWidget(this);
	
	SummaryFilterWidget* errorsFilterWidget = new SummaryFilterWidget(m_stackedWidget);
	errorsFilterWidget->setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType::ErrorsFilterPage);

	SummaryFilterWidget* allResourcesPage = new SummaryFilterWidget(m_stackedWidget);
	allResourcesPage->setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType::AllResourcesPage);

	TableView* crawlingTableView = new TableView(m_stackedWidget);

	PageInfoStorageModel* model = new PageInfoStorageModel(this);
	PageInfoStorageViewModel* modelView = new PageInfoStorageViewModel(model, this);

	model->setStorageAdaptor(theApp->storageAdaptorFactory()->create(SummaryCategoryItem::SummaryCategoryItemAllPages));

	crawlingTableView->setModel(model);
	crawlingTableView->setViewModel(modelView);
	crawlingTableView->setContextMenu(new ContextMenuDataCollectionRow(crawlingTableView));

	m_pageIndexes[Page::SeoAnalysisPage] = m_stackedWidget->addWidget(errorsFilterWidget);
	m_pageIndexes[Page::AllResourcesPage] = m_stackedWidget->addWidget(allResourcesPage);
	m_pageIndexes[Page::AllPagesPage] = m_stackedWidget->addWidget(crawlingTableView);
}

}