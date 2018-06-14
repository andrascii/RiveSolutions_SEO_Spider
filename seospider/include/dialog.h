#pragma once

namespace SeoSpider
{

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget* parent = nullptr);

	virtual void open() override;
	virtual int exec() override;
};

}