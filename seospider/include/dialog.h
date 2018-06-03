#pragma once

#include "idialog.h"
#include "ui_message_box.h"

namespace SeoSpider
{

class Dialog : public QFrame, public IDialog
{
	Q_OBJECT

//friend class FloatingDialog;

public:
	Dialog(QWidget* decoratedWidget, QWidget* parent = nullptr);
	virtual ~Dialog() = default;

	//IDialog implementaion
signals:
	virtual void dialogClosed(int clickedButtonRole) override;

public slots:
	virtual void accept() override;
	virtual void reject() override;
	virtual void open() override;
	virtual void done(int r) override;
	virtual int result() const override;

private:
	void completeLocalEventLoop();

private:
	QDialog::DialogCode m_dialogCode;
	QEventLoop m_eventLoop;
};

}
