#pragma once

namespace SeoSpider
{
	
class IDialog
{
public:
	virtual ~IDialog() = default;

	//public signals
	virtual void dialogClosed(int clickedButtonRole) = 0;

	//public slots
	virtual void accept() = 0;
	virtual void reject() = 0;
	virtual void open() = 0;
	virtual void done(int r) = 0;
	virtual int result() const = 0;
};

}