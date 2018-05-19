#include "content_frame.h"
#include "application.h"
#include "helpers.h"
#include "header_decoration_widget.h"
#include "control_panel_widget.h"
#include "crawler_progress_bar.h"
#include "custom_push_button.h"
#include "header_controls_container.h"
#include "main_window.h"
#include "wait_operation_frame.h"

namespace SeoSpider
{

ContentFrame::ContentFrame(QWidget* parent)
	: QFrame(parent)
	, m_decorationWidget(new HeaderDecorationWidget(this))
{
	initializeNavigationPanelWidget();

	m_stackedWidget = new QStackedWidget(this);

	m_decorationWidget->addWidgetToHeader(new ControlPanelWidget(this));

	QHBoxLayout* dynamicControlsLayoutWithSpacer = new QHBoxLayout(this);
	dynamicControlsLayoutWithSpacer->setMargin(0);
	m_dynamicControlsLayout = new QHBoxLayout(this);
	m_dynamicControlsLayout->setMargin(0);

	QWidget* dynamicControlsWidget = new QFrame(this);
	dynamicControlsWidget->setContentsMargins(0, 0, 0, 0);
	dynamicControlsWidget->setObjectName(QStringLiteral("DynamicControls"));
	dynamicControlsWidget->setLayout(dynamicControlsLayoutWithSpacer);
	dynamicControlsLayoutWithSpacer->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	dynamicControlsLayoutWithSpacer->addLayout(m_dynamicControlsLayout);
	
	m_decorationWidget->addWidgetToHeader(dynamicControlsWidget, Qt::AlignRight, false);
	m_decorationWidget->addContentWidget(m_navigationPanel.navigationPanelWidget);
	m_decorationWidget->addContentWidget(m_stackedWidget);

	QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setMargin(0);
	horizontalLayout->addWidget(m_decorationWidget);

	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::stateChanged,
		this, &ContentFrame::onStateChanged));

	VERIFY(connect(theApp->mainWindow(), SIGNAL(showPage(PageFactory::Page)),
		this, SLOT(showPage(PageFactory::Page))));

	QWidget* loadingWidget = new QWidget(this);
	QHBoxLayout* loadingWidgetLayout = new QHBoxLayout(loadingWidget);
	loadingWidgetLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Expanding));

	QLabel* label = new QLabel(this);
	label->setText(tr("Please wait..."));
	label->setObjectName("LoadingWidget");
	loadingWidgetLayout->addWidget(label);
	loadingWidgetLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Expanding));
	
	m_stackedWidget->addWidget(loadingWidget);

	VERIFY(connect(theApp->headerControlsContainer(), &HeaderControlsContainer::currentControlsChanged, 
		this, &ContentFrame::onDynamicControlsChanged));
}

void ContentFrame::addPage(PageFactory::Page page, QWidget* widget, const QString& buttonText, const QIcon& buttonIcon, bool setSelected)
{
	m_pageIndexes[page] = m_stackedWidget->addWidget(widget);

	m_navigationPanel.pushButtons[page] = new CustomPushButton(buttonIcon, buttonText, m_navigationPanel.navigationPanelWidget);
	m_navigationPanel.pushButtons[page]->setIconSize(QSize(20, 20));
	
	VERIFY(connect(m_navigationPanel.pushButtons[page], &QPushButton::clicked,
		this, &ContentFrame::handleNavigationPanelButtonClick));

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

QWidget* ContentFrame::page(PageFactory::Page page) const noexcept
{
	const int pageIndex = m_pageIndexes.value(page, -1);

	return pageIndex == -1 ? nullptr : m_stackedWidget->widget(pageIndex);
}

void ContentFrame::showPage(PageFactory::Page page)
{
	if (m_prevButton)
	{
		m_prevButton->setProperty("selected", false);
		m_prevButton->repaint();
		m_prevButton->style()->unpolish(m_prevButton);
		m_prevButton->style()->polish(m_prevButton);
	}

	QPushButton* button = m_navigationPanel.pushButtons[page];

	button->setProperty("selected", true);
	button->style()->unpolish(button);
	button->style()->polish(button);

	m_prevButton = button;

	m_stackedWidget->setCurrentIndex(m_pageIndexes[page]);
	m_activePage = page;
	theApp->headerControlsContainer()->setActivePage(page);
}

void ContentFrame::handleNavigationPanelButtonClick()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	DEBUG_ASSERT(button);

	if (m_prevButton == button)
	{
		return;
	}

	auto pushButtonsIterator = std::find_if(
		std::begin(m_navigationPanel.pushButtons),
		std::end(m_navigationPanel.pushButtons), 
		[button](const auto& pair) { return pair.second == button; }
	);

	DEBUG_ASSERT(pushButtonsIterator != std::end(m_navigationPanel.pushButtons));

	showPage(pushButtonsIterator->first);
}

void ContentFrame::onStateChanged(int state)
{
	if (state == CrawlerEngine::Crawler::State::StateSerializaton ||
		state == CrawlerEngine::Crawler::State::StateDeserializaton)
	{
		WaitOperationFrame::showMessage(tr("Please wait while saving the project..."));

		m_stackedWidget->setCurrentIndex(0);
	}
	else
	{
		WaitOperationFrame::finish();

		showPage(m_activePage);
	}
}

void ContentFrame::initializeNavigationPanelWidget()
{
	m_navigationPanel.navigationPanelWidget = new QWidget(this);
	
	QVBoxLayout* layout = new QVBoxLayout(m_navigationPanel.navigationPanelWidget);
	
	layout->setContentsMargins(0, 0, Common::Helpers::pointsToPixels(0.6), 0);

	layout->setSpacing(0);

	layout->addItem(
		new QSpacerItem(
			Common::Helpers::pointsToPixels(15),
			Common::Helpers::pointsToPixels(30), 
			QSizePolicy::Minimum, QSizePolicy::Expanding)
	);

	// use this name for customize style through stylesheets
	m_navigationPanel.navigationPanelWidget->setObjectName(QStringLiteral("NavigationPanel"));
}

void ContentFrame::onDynamicControlsChanged(int page)
{
	const PageFactory::Page currentPage = static_cast<PageFactory::Page>(page);
	QList<QWidget*> controls = theApp->headerControlsContainer()->controls(currentPage);

	QLayoutItem *item;
	while ((item = m_dynamicControlsLayout->itemAt(0))) 
	{
		if (item->widget())
		{
			item->widget()->hide();
		}

		m_dynamicControlsLayout->removeItem(item);
		delete item;
	}

	foreach(QWidget* control, controls)
	{
		m_dynamicControlsLayout->addWidget(control);
		control->show();
	}
}

}
