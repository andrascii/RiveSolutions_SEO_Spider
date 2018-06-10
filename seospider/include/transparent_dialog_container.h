#pragma once

namespace SeoSpider
{

class Dialog;

class TransparentDialogContainer : public QFrame
{
public:
	TransparentDialogContainer(Dialog* dialog);
	~TransparentDialogContainer();

protected:
	virtual void closeEvent(QCloseEvent* event) override;
};

}