#include "stdafx.h"
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
	m_ui.robotsTxtComboBox->addItem("AnyBot", QVariant::fromValue(CrawlerEngine::UserAgentType::AnyBot));
	m_ui.robotsTxtComboBox->addItem("GoogleBot", QVariant::fromValue(CrawlerEngine::UserAgentType::GoogleBot));
	m_ui.robotsTxtComboBox->addItem("YandexBot", QVariant::fromValue(CrawlerEngine::UserAgentType::YandexBot));
	m_ui.robotsTxtComboBox->addItem("MailRuBot", QVariant::fromValue(CrawlerEngine::UserAgentType::MailRuBot));
	m_ui.robotsTxtComboBox->addItem("YahooBot", QVariant::fromValue(CrawlerEngine::UserAgentType::YahooBot));

	SettingsPage::init();

	m_ui.robotsTxtComboBox->setEnabled(m_ui.followRobotsTxtCheckBox->isChecked());

	connect(m_ui.checkExternalLinksCheckBox, &QCheckBox::toggled, m_ui.followExternalNofollowCheckBox, &QWidget::setEnabled);
	m_ui.followExternalNofollowCheckBox->setEnabled(m_ui.checkExternalLinksCheckBox->isChecked());
}

void CrawlerSettingsWidget::onShow()
{
}

void CrawlerSettingsWidget::onClose()
{
}

}