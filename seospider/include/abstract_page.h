#pragma once

#include "ipage.h"

namespace SeoSpider
{

class AbstractPage : public IPage
{
public:
	virtual QList<QWidget*> bindControls() const override;
	virtual void addWidget(QWidget* control, int position = -1) override;
	virtual void addAction(QAction* action, int position = -1) override;
	virtual void showMe() const override;

protected:
	QWidget* createControl(QAction* action) const;

private:
	QList<QWidget*> m_controls;
	QMap<QAction*, QWidget*>  m_controlsByAction;
};

}