#pragma once

#include "ui_message_box.h"
#include "floating_frame.h"

namespace SeoSpider
{

class MessageBoxDialog : public FloatingFrame
{
	Q_OBJECT

public:
	MessageBoxDialog(QWidget* parent = nullptr);
	~MessageBoxDialog();

	void setMessage(const QString& message);
	void setStandardButtons(QDialogButtonBox::StandardButtons buttons);
	int result() const;

	void addButton(const QString &text, QDialogButtonBox::ButtonRole role);

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
	virtual bool event(QEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	void completeLocalEventLoop();

private:
	Ui_MessageBox * m_ui;
	QDialog::DialogCode m_dialogCode;
	QDialogButtonBox::ButtonRole m_clickedButtonRole;
	QEventLoop* m_eventLoop;
};

}
