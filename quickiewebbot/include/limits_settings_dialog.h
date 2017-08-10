#pragma once

#include "ui_limits_settings.h"

namespace QuickieWebBot
{

class LimitsSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	LimitsSettingsDialog(QWidget* parent = nullptr);

private:
	void initialize();

private:
	Ui::LimitsSettingsDialog m_ui;
};

}
