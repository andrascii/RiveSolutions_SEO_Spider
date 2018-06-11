#pragma once

namespace SeoSpider
{

class Dialog;
class FloatingDialog;

class TransparentDialogContainer : public QDialog
{
	Q_OBJECT

public:
	TransparentDialogContainer(Dialog* dialog);
	TransparentDialogContainer(FloatingDialog* dialog);

protected:
	virtual void showEvent(QShowEvent* event) override;

private:
	TransparentDialogContainer(QWidget* dialog);
};

}