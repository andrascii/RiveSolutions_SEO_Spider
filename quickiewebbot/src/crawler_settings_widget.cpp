#include "crawler_settings_widget.h"
#include "application_properties.h"

namespace QuickieWebBot
{

CrawlerSettingsWidget::CrawlerSettingsWidget(QWidget* parent)
	: QWidget(parent)
{
	initialize();
}

void CrawlerSettingsWidget::initialize()
{
	m_ui.setupUi(this);
}

void CrawlerSettingsWidget::applyChanges() noexcept
{
	if (isAutoApply())
	{
		return;
	}

	theApp->properties()->setCheckImages(m_ui.checkImages->isChecked());
	theApp->properties()->setCheckCanonicals(m_ui.checkCanonicals->isChecked());
}

bool CrawlerSettingsWidget::isAutoApply() const noexcept
{
	return false;
}

}