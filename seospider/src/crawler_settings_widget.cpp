#include "crawler_settings_widget.h"
#include "parsed_page.h"

namespace SeoSpider
{

CrawlerSettingsWidget::CrawlerSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	init();

	applyChanges();
}

void CrawlerSettingsWidget::init()
{
	m_ui.label->installEventFilter(this);
	m_ui.label_2->installEventFilter(this);
	m_ui.label_3->installEventFilter(this);
	m_ui.label_4->installEventFilter(this);
	m_ui.label_5->installEventFilter(this);
	m_ui.label_6->installEventFilter(this);
	m_ui.label_7->installEventFilter(this);
	m_ui.label_8->installEventFilter(this);
	m_ui.label_9->installEventFilter(this);
	m_ui.label_10->installEventFilter(this);
	m_ui.label_11->installEventFilter(this);
	m_ui.label_12->installEventFilter(this);

	m_ui.robotsTxtComboBox->addItem("AnyBot", QVariant::fromValue(CrawlerEngine::UserAgentType::AnyBot));
	m_ui.robotsTxtComboBox->addItem("GoogleBot", QVariant::fromValue(CrawlerEngine::UserAgentType::GoogleBot));
	m_ui.robotsTxtComboBox->addItem("YandexBot", QVariant::fromValue(CrawlerEngine::UserAgentType::YandexBot));
	m_ui.robotsTxtComboBox->addItem("MailRuBot", QVariant::fromValue(CrawlerEngine::UserAgentType::MailRuBot));
	m_ui.robotsTxtComboBox->addItem("YahooBot", QVariant::fromValue(CrawlerEngine::UserAgentType::YahooBot));

	SettingsPage::init();

	m_ui.robotsTxtComboBox->setEnabled(m_ui.followRobotsTxtCheckBox->isChecked());
}

bool CrawlerSettingsWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_ui.label && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.crawlOutsideStartFolderCheckBox->toggle();
	}

	if (object == m_ui.label_2 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkSwfCheckBox->toggle();
	}

	if (object == m_ui.label_3 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkJavaScriptCheckBox->toggle();
	}

	if (object == m_ui.label_4 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkCssCheckBox->toggle();
	}

	if (object == m_ui.label_5 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkImagesCheckBox->toggle();
	}

	if (object == m_ui.label_6 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.followExternalNofollowCheckBox->toggle();
	}

	if (object == m_ui.label_7 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkCanonicalsCheckBox->toggle();
	}

	if (object == m_ui.label_8 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkSubdomainsCheckBox->toggle();
	}

	if (object == m_ui.label_9 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.followRobotsTxtCheckBox->toggle();
	}

	if (object == m_ui.label_10 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.checkExternalLinksCheckBox->toggle();
	}

	if (object == m_ui.label_11 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.followInternalNofollowCheckBox->toggle();
	}

	if (object == m_ui.label_12 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.crawlMetaHerefLangLinksCheckBox->toggle();
	}

	return false;
}

void CrawlerSettingsWidget::onShow()
{
}

void CrawlerSettingsWidget::onClose()
{
}

}