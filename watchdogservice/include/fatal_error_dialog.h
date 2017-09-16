#pragma once

#include "ui_fatal_error_dialog.h"

namespace WatchDogService
{

class FatalErrorDialog : public QDialog
{
	Q_OBJECT

public:
	FatalErrorDialog(QWidget* parent = nullptr);

private:
	Ui::FatalErrorDialog* m_ui;
};

}