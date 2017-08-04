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

void ControlPanelWidget::setUrl()
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	theApp->properties()->setUrl(m_ui.urlLineEdit->text());
}

void ControlPanelWidget::startCrawling()
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	theApp->webCrawler()->startCrawling();
}

void ControlPanelWidget::initialize()
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::startCrawling));
	VERIFY(connect(m_ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::stopCrawling));

	VERIFY(connect(m_ui.urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setUrl));

	m_ui.viewTypeComboBox->addItem(tr("List"));
	m_ui.viewTypeComboBox->addItem(tr("Tree"));
}

}