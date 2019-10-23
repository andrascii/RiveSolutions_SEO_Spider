#include "stdafx.h"
#include "page_visual_settings_widget.h"


namespace SeoSpider
{

PageVisualSettingsWidget::PageVisualSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.useCustomBackgroundForNotIndexPagesCheckBox, &QCheckBox::toggled, 
		m_ui.notIndexedPagesColorLabel, &QWidget::setEnabled));

	VERIFY(connect(m_ui.useCustomBackgroundForNotIndexPagesCheckBox, &QCheckBox::toggled,
		m_ui.colorSelector, &QWidget::setEnabled));

	m_ui.notIndexedPagesColorLabel->setEnabled(m_ui.useCustomBackgroundForNotIndexPagesCheckBox->isChecked());
	m_ui.colorSelector->setEnabled(m_ui.useCustomBackgroundForNotIndexPagesCheckBox->isChecked());

	init();
}

void PageVisualSettingsWidget::onClose()
{
}

void PageVisualSettingsWidget::init()
{
	SettingsPage::init();
}

void PageVisualSettingsWidget::onShow()
{
}

}
