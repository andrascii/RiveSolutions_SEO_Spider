#include "crawler_control_widget.h"
#include "application.h"
#include "web_crawler.h"

namespace QuickieWebBot
{

CrawlerControlWidget::CrawlerControlWidget(QWidget* parent)
	: QWidget(parent)
{
	initialize();
}

void CrawlerControlWidget::showListView(bool value)
{
	m_ui.summaryViewButton->setChecked(!value);

	if (value)
	{
		theApp->mainFrame()->showListView();
	}
}

void CrawlerControlWidget::showSummaryView(bool value)
{
	m_ui.generalViewButton->setChecked(!value);

	if (value)
	{
		theApp->mainFrame()->showSummaryView();
	}
}

void CrawlerControlWidget::initialize()
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.generalViewButton, &QPushButton::toggled, this, &CrawlerControlWidget::showListView));
	VERIFY(connect(m_ui.summaryViewButton, &QPushButton::toggled, this, &CrawlerControlWidget::showSummaryView));

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::startCrawling));
	VERIFY(connect(m_ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::stopCrawling));

	m_ui.viewTypeComboBox->addItem(tr("List"));
	m_ui.viewTypeComboBox->addItem(tr("Tree"));
}

}