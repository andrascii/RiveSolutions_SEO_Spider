#include "application.h"
#include "application_properties.h"
#include "limits_settings_dialog.h"

namespace QuickieWebBot
{

LimitsSettingsDialog::LimitsSettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	initialize();
}

void LimitsSettingsDialog::initialize()
{
	m_ui.setupUi(this);
}

}