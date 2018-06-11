#pragma once

#include "idialog.h"
#include "ui_message_box.h"

namespace SeoSpider
{

class Dialog : public QFrame, public IDialog
{
	Q_OBJECT

public:
	Dialog(QWidget* parent = nullptr);

	virtual QObject* qobject() const override;

signals:
	virtual void dialogClosed(int clickedButtonRole) override;

public slots:
	virtual void accept() override;
	virtual void reject() override;
	virtual void open() override;
	virtual void done(int r) override;
	virtual int result() const override;

protected:
	virtual void hideEvent(QHideEvent* event) override;

private:
	QDialog::DialogCode m_dialogCode;
};

}
