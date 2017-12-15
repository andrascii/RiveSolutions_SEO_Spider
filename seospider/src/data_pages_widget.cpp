#include "data_pages_widget.h"
#include "application.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "storage_adapter_factory.h"
#include "context_menu_data_collection_row.h"
#include "seo_spider_helpers.h"
#include "header_decoration_widget.h"
#include "control_panel_widget.h"
#include "filter_widget.h"
#include "website_data_widget.h"
#include "page_data_widget.h"
#include "crawler_progress_bar.h"
#include "custom_push_button.h"

namespace SeoSpider
{

DataPagesWidget::DataPagesWidget(QWidget* parent)
	: QFrame(parent)
	, m_decorationWidget(new HeaderDecorationWidget(this))
{
	initializeNavigationPanelWidget();

	m_stackedWidget = new QStackedWidget(this);

	m_decorationWidget->addWidgetToHeader(new ControlPanelWidget(this));
	m_decorationWidget->addWidgetToHeader(new CrawlerProgressBar(this), Qt::AlignLeft, true);
	m_decorationWidget->setContentWidget(m_stackedWidget);

	QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setMargin(0);

	horizontalLayout->addWidget(m_navigationPanel.navigationPanelWidget);
	horizontalLayout->addWidget(m_decorationWidget);

	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::stateChanged,
		this, &DataPagesWidget::onStateChanged));

	QWidget* loadingWidget = new QWidget(this);
	QHBoxLayout* loadingWidgetLayout = new QHBoxLayout(this);
	loadingWidget->setLayout(loadingWidgetLayout);
	loadingWidgetLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Expanding));
	QLabel* label = new QLabel(this);
	label->setText(tr("Please wait..."));
	label->setObjectName("LoadingWidget");
	loadingWidgetLayout->addWidget(label);
	loadingWidgetLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Expanding));
	
	m_stackedWidget->addWidget(loadingWidget);
}

void DataPagesWidget::addPage(PageFactory::Page page, QWidget* widget, const QString& buttonText, const QIcon& buttonIcon, bool setSelected)
{
	m_pageIndexes[page] = m_stackedWidget->addWidget(widget);

	m_navigationPanel.pushButtons[page] = new CustomPushButton(buttonIcon, buttonText, m_navigationPanel.navigationPanelWidget);

	VERIFY(connect(m_navigationPanel.pushButtons[page], &QPushButton::clicked,
		this, &DataPagesWidget::handleNavigationPanelButtonClick));

	const int insertPosition = m_navigationPanel.navigationPanelWidget->layout()->count() - 1;

	QBoxLayout* layout = qobject_cast<QBoxLayout*>(m_navigationPanel.navigationPanelWidget->layout());

	ASSERT(layout);

	layout->insertWidget(insertPosition, m_navigationPanel.pushButtons[page]);

	m_navigationPanel.pushButtons[page]->setProperty("selected", setSelected);

	if (setSelected)
	{
		m_prevButton = m_navigationPanel.pushButtons[page];
		showPage(page);
	}
}

void DataPagesWidget::showPage(PageFactory::Page page)
{
	m_stackedWidget->setCurrentIndex(m_pageIndexes[page]);
	m_activePage = page;
}

void DataPagesWidget::handleNavigationPanelButtonClick()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	DEBUG_ASSERT(button);

	if (m_prevButton == button)
	{
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

void DataPagesWidget::onStateChanged(int state)
{
	if (state == CrawlerEngine::Crawler::State::StateSerializaton)
	{
		m_stackedWidget->setCurrentIndex(0);
		m_stackedWidget->setEnabled(false);
		m_decorationWidget->setEnabled(false);
		m_navigationPanel.navigationPanelWidget->setEnabled(false);
	}
	else
	{
		m_stackedWidget->setEnabled(true);
		m_decorationWidget->setEnabled(true);
		m_navigationPanel.navigationPanelWidget->setEnabled(true);
		showPage(m_activePage);
	}
}

void DataPagesWidget::initializeNavigationPanelWidget()
{
	m_navigationPanel.navigationPanelWidget = new QWidget(this);
	
	QVBoxLayout* layout = new QVBoxLayout(m_navigationPanel.navigationPanelWidget);
	
	layout->setContentsMargins(0, 0, SeoSpiderHelpers::pointsToPixels(0.6), 0);

	layout->setSpacing(0);

	layout->addItem(
		new QSpacerItem(
			SeoSpiderHelpers::pointsToPixels(15),
			SeoSpiderHelpers::pointsToPixels(30), 
			QSizePolicy::Minimum, QSizePolicy::Expanding)
	);

	// use this name for customize style through stylesheets
	m_navigationPanel.navigationPanelWidget->setObjectName(QStringLiteral("NavigationPanel"));
}

}