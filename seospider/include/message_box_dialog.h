#pragma once

#include "ui_message_box.h"
#include "floating_frame.h"
#include "dialog.h"
#include "floating_dialog.h"

namespace SeoSpider
{

class MessageBoxDialog : public Dialog
{
	Q_OBJECT

public:
	MessageBoxDialog(QWidget* parent = nullptr);

	void setMessage(const QString& message);
	void setStandardButtons(QDialogButtonBox::StandardButtons buttons);

private slots:
	void onButtonClicked(QAbstractButton* button);

protected:
	virtual void showEvent(QShowEvent* event) override;
	//virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	Ui_MessageBox* m_ui;
	QDialog::DialogCode m_dialogCode;
	QDialogButtonBox::ButtonRole m_clickedButtonRole;
};

}
