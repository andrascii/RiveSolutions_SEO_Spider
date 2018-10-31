#pragma once

namespace SeoSpider
{

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget* parent = nullptr);

	static Dialog* shownDialog();

	virtual void open() override;
	virtual int exec() override;

signals:
	void dialogClosed();

private:
	static Dialog* s_dialog;
};

}