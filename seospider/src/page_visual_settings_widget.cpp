#include "page_visual_settings_widget.h"


namespace SeoSpider
{

PageVisualSettingsWidget::PageVisualSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);
	init();
}

bool PageVisualSettingsWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_ui.label && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.useCustomBackgroundForNotIndexPagesCheckBox->toggle();
	}

	return false;
}

void PageVisualSettingsWidget::init()
{
	m_ui.label->installEventFilter(this);
	SettingsPage::init();
}

}
