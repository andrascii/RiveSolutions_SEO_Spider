#pragma once

#include "ui_message_box_dialog.h"

namespace QuickieWebBot
{

class MessageBoxDialog : public QDialog
{
public:
	enum Icon
	{
		InformationIcon,
		WarningIcon,
		CriticalErrorIcon
	};

	MessageBoxDialog(QWidget* parent = nullptr);

	void setMessage(const QString& message);
	void setIcon(Icon icon);

private:
	Ui_MessageBoxDialog* m_ui;
};

}
