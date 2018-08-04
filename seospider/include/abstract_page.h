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
	virtual void goTo(PageDataWidget::PageDataType pageDataType) override;

protected:
	QWidget* createControl(QAction* action) const;

protected:
	static const QString s_noResultsMessageStub;
	PageDataWidget* m_pageDataWidget = nullptr;

private:
	QList<QWidget*> m_controls;
	QMap<QAction*, QWidget*>  m_controlsByAction;
};

}