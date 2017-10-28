#pragma once

#include "ui_message_box.h"

namespace QuickieWebBot
{

class MessageBoxDialog : public QFrame
{
	Q_OBJECT

public:
	enum Icon
	{
		InformationIcon,
		WarningIcon,
		CriticalErrorIcon
	};

	MessageBoxDialog(QWidget* parent = nullptr);

	void setMessage(const QString& message) const;
	void setIcon(Icon icon) const;

	void setStandardButtons(QDialogButtonBox::StandardButtons buttons);

	int	result() const;

public slots:
	virtual void accept();
	virtual void reject();
	virtual void done(int r);

protected:
	virtual void showEvent(QShowEvent* event) override;

private:
	Ui_MessageBox* m_ui;

	QDialog::DialogCode m_dialogCode;
};

}
