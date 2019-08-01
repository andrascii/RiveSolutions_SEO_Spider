#include "stdafx.h"
#include "limits_settings_widget.h"
#include "application.h"
#include "command_line_handler.h"
#include "command_line_keys.h"

namespace SeoSpider
{

LimitsSettingsWidget::LimitsSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	init();

	applyChanges();
}

void LimitsSettingsWidget::init()
{
	SettingsPage::init();

	if (theApp->commandLineHandler()->hasCommand(s_useOldDownloader))
	{
		m_ui.maxConnectionsLabel->setEnabled(false);
		m_ui.maxConnectionsSpinBox->setEnabled(false);
	}
}

void LimitsSettingsWidget::onShow()
{
}

void LimitsSettingsWidget::onClose()
{
}
}
