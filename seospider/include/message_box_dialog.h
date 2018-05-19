#pragma once

#include "ui_message_box.h"
#include "floating_frame.h"

namespace SeoSpider
{

class MessageBoxDialog : public FloatingFrame
{
	Q_OBJECT

public:
	MessageBoxDialog();

	void setMessage(const QString& message);
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
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	void completeLocalEventLoop();

private:
	Ui_MessageBox* m_ui;
	QDialog::DialogCode m_dialogCode;
	QDialogButtonBox::ButtonRole m_clickedButtonRole;
	QEventLoop m_eventLoop;
};

}
