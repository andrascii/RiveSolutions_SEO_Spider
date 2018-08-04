#include "content_frame.h"
#include "application.h"
#include "helpers.h"
#include "header_decoration_widget.h"
#include "control_panel_widget.h"
#include "crawler_progress_bar.h"
#include "custom_push_button.h"
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

	m_dynamicControlsLayout = new QHBoxLayout;
	m_dynamicControlsLayout->setMargin(0);

	QWidget* dynamicControlsWidget = new QFrame(this);
	dynamicControlsWidget->setContentsMargins(0, 0, 0, 0);
	dynamicControlsWidget->setObjectName(QStringLiteral("DynamicControls"));
	QHBoxLayout* dynamicControlsLayoutWithSpacer = new QHBoxLayout(this);
	dynamicControlsLayoutWithSpacer->setMargin(0);
	dynamicControlsLayoutWithSpacer->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	dynamicControlsLayoutWithSpacer->addLayout(m_dynamicControlsLayout);
	dynamicControlsWidget->setLayout(dynamicControlsLayoutWithSpacer);

	m_decorationWidget->addWidgetToHeader(dynamicControlsWidget, Qt::AlignRight, false);
	m_decorationWidget->addContentWidget(m_navigationPanel.navigationPanelWidget);
	m_decorationWidget->addContentWidget(m_stackedWidget);

	QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setMargin(0);
	horizontalLayout->addWidget(m_decorationWidget);

	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::stateChanged,
		this, &ContentFrame::onStateChanged));
}

void ContentFrame::addPage(IPage* page, bool setSelected)
{
	m_pageIndexes[page->type()] = m_stackedWidget->addWidget(page->widget());

	m_navigationPanel.pushButtons[page->type()] = new CustomPushButton(page->icon(), page->name(), m_navigationPanel.navigationPanelWidget);
	m_navigationPanel.pushButtons[page->type()]->setIconSize(QSize(20, 20));
	
	VERIFY(connect(m_navigationPanel.pushButtons[page->type()], &QPushButton::clicked,
		this, &ContentFrame::handleNavigationPanelButtonClick));

	const int insertPosition = m_navigationPanel.navigationPanelWidget->layout()->count() - 1;

	QBoxLayout* layout = qobject_cast<QBoxLayout*>(m_navigationPanel.navigationPanelWidget->layout());

	ASSERT(layout);

	layout->insertWidget(insertPosition, m_navigationPanel.pushButtons[page->type()]);

	m_navigationPanel.pushButtons[page->type()]->setProperty("selected", setSelected);

	if (setSelected)
	{
		m_prevButton = m_navigationPanel.pushButtons[page->type()];
		showPage(page->type());
	}

	VERIFY(connect(page->widget(), SIGNAL(controlsChanged(IPage::Type)), this, SLOT(onDynamicControlsChanged(IPage::Type))));
}

IPage* ContentFrame::page(IPage::Type pageType) const
{
	return dynamic_cast<IPage*>(pageWidget(pageType));
}

QWidget* ContentFrame::pageWidget(IPage::Type pageType) const
{
	auto iterator = m_pageIndexes.find(pageType);

	if (iterator == std::end(m_pageIndexes))
	{
		return nullptr;
	}

	return m_stackedWidget->widget(iterator.value());
}

void ContentFrame::showPage(IPage::Type pageType)
{
	if (m_prevButton)
	{
		m_prevButton->setProperty("selected", false);
		m_prevButton->repaint();
		m_prevButton->style()->unpolish(m_prevButton);
		m_prevButton->style()->polish(m_prevButton);
	}

	QPushButton* button = m_navigationPanel.pushButtons[pageType];

	button->setProperty("selected", true);
	button->style()->unpolish(button);
	button->style()->polish(button);

	m_prevButton = button;

	m_stackedWidget->setCurrentIndex(m_pageIndexes[pageType]);
	m_activePage = pageType;

	onDynamicControlsChanged(pageType);
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
	static bool waitOperationFrameShown = false;

	const bool isSavingOrLoadingProject = state == CrawlerEngine::Crawler::State::StateSerializaton ||
		state == CrawlerEngine::Crawler::State::StateDeserializaton;

	QString userWaitMessage;

	if (state == CrawlerEngine::Crawler::State::StateSerializaton)
	{
		userWaitMessage = tr("Please wait while saving the project...");
	}

	if (state == CrawlerEngine::Crawler::State::StateDeserializaton)
	{
		userWaitMessage = tr("Please wait while loading the project...");
	}

	if (isSavingOrLoadingProject)
	{
		WaitOperationFrame::showMessage(userWaitMessage);

		waitOperationFrameShown = true;
	}
	else
	{
		if (waitOperationFrameShown)
		{
			WaitOperationFrame::finish();
			waitOperationFrameShown = false;
		}
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

void ContentFrame::onDynamicControlsChanged(IPage::Type pageType)
{
	QList<QWidget*> controls = page(pageType)->bindControls();

	QLayoutItem* item;

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
		const QVariant reservedForFuture = control->property("ReservedForFuture");

		if (reservedForFuture.isValid())
		{
			continue;
		}

		m_dynamicControlsLayout->addWidget(control);

		control->show();
	}
}

}
