#pragma once

#include "ui_message_box.h"

namespace SeoSpider
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

	MessageBoxDialog();

	void setMessage(const QString& message);
	void setIcon(Icon icon);

	void setStandardButtons(QDialogButtonBox::StandardButtons buttons);

	int	result() const;

signals:
	void dialogClosed(int clickedButtonRole);

public slots:
	virtual void accept();
	virtual void reject();
	virtual void done(int r);

private slots:
	void onButtonClicked(QAbstractButton* button);

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	Ui_MessageBox* m_ui;

	QDialog::DialogCode m_dialogCode;

	QDialogButtonBox::ButtonRole m_clickedButtonRole;
};

}
