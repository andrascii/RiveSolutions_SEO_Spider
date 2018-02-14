#include "page_visual_settings_widget.h"


namespace SeoSpider
{

PageVisualSettingsWidget::PageVisualSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
	, m_colorSelector(this)
{
	m_ui.setupUi(this);

	m_colorSelector.setProperty("controlKey", "notIndexedPagesColor");

	init();

	VERIFY(connect(m_ui.SelectColorPushButton, SIGNAL(clicked()),
		this, SLOT(selectColorButtonClicked())));
}

void PageVisualSettingsWidget::selectColorButtonClicked()
{
	m_notIndexedPagesColor = QColorDialog::getColor();
	m_colorSelector.setColor(m_notIndexedPagesColor);

	QString qss = QString("background-color: %1").arg(m_notIndexedPagesColor.name());
	m_ui.SelectColorPushButton->setStyleSheet(qss);
}

void PageVisualSettingsWidget::init()
{
	SettingsPage::init();
}

}
