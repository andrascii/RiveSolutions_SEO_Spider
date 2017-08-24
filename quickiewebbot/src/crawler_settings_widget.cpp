#include "crawler_settings_widget.h"

namespace QuickieWebBot
{

CrawlerSettingsWidget::CrawlerSettingsWidget(QWidget* parent)
	: QFrame(parent)
{
	initialize();
}

void CrawlerSettingsWidget::initialize()
{
	m_ui.setupUi(this);
}

}