#include "application.h"
#include "application_properties.h"
#include "control_panel_widget.h"
#include "web_crawler.h"

namespace QuickieWebBot
{

ControlPanelWidget::ControlPanelWidget(QWidget* parent)
	: QWidget(parent)
{
	initialize();
}

void ControlPanelWidget::showListView(bool value)
{
	m_ui.summaryViewButton->setChecked(!value);

	if (value)
	{
		theApp->mainFrame()->showListView();
	}
}

void ControlPanelWidget::showSummaryView(bool value)
{
	m_ui.generalViewButton->setChecked(!value);

	if (value)
	{
		theApp->mainFrame()->showSummaryView();
	}
}

void ControlPanelWidget::setHost()
{
	theApp->properties()->setUrl(m_ui.urlLineEdit->text());
}

void ControlPanelWidget::startCrawling()
{
	if (QUrl(m_ui.urlLineEdit->text()).isValid())
	{
		theApp->webCrawler()->startCrawling();
	}
}

void ControlPanelWidget::initialize()
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.generalViewButton, &QPushButton::toggled, this, &ControlPanelWidget::showListView));
	VERIFY(connect(m_ui.summaryViewButton, &QPushButton::toggled, this, &ControlPanelWidget::showSummaryView));

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::startCrawling));
	VERIFY(connect(m_ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::stopCrawling));

	VERIFY(connect(m_ui.urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setHost));

	m_ui.viewTypeComboBox->addItem(tr("List"));
	m_ui.viewTypeComboBox->addItem(tr("Tree"));
}

}