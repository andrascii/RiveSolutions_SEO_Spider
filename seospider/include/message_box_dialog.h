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

	int result() const;

signals:
	void dialogClosed(int clickedButtonRole);

public slots:
	void accept();
	void reject();
	void done(int r);
	void exec();

private slots:
	void onButtonClicked(QAbstractButton* button);

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	void completeLocalEventLoop();

private:
	Ui_MessageBox* m_ui;

	QDialog::DialogCode m_dialogCode;

	QDialogButtonBox::ButtonRole m_clickedButtonRole;

	QEventLoop m_eventLoop;
};

}
