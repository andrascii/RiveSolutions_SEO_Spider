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

	// set properties

	WebCrawler::WebCrawlerOptions options;
	options.url = theApp->properties()->url();
	options.minTitleLength = theApp->properties()->minTitleLength();
	options.maxTitleLength = theApp->properties()->maxTitleLength();
	options.limitMaxUrlLength = theApp->properties()->limitMaxUrlLength();
	options.maxDescriptionLength = theApp->properties()->maxDescriptionLength();
	options.minDescriptionLength = theApp->properties()->minDescriptionLength();
	options.maxH1LengthChars = theApp->properties()->maxH1LengthChars();
	options.maxH2LengthChars = theApp->properties()->maxH2LengthChars();
	options.maxImageAltTextChars = theApp->properties()->maxImageAltTextChars();
	options.maxImageSizeKb = theApp->properties()->maxImageSize();


	theApp->webCrawler()->startCrawling(options);
}

void ControlPanelWidget::initialize()
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::startCrawling));
	VERIFY(connect(m_ui.stopCrawlingButton, &QPushButton::clicked, theApp->webCrawler(), &WebCrawler::WebCrawler::stopCrawling));

	VERIFY(connect(m_ui.urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setUrl));

	m_ui.viewTypeComboBox->addItem(tr("List"));
	m_ui.viewTypeComboBox->addItem(tr("Tree"));
}

}