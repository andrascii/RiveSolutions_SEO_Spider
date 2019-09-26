#pragma once

#include "abstract_page.h"

namespace SeoSpider
{

class AbstractTablePage: public QFrame, public AbstractPage
{
	Q_OBJECT

public:
	AbstractTablePage(QWidget* parent, PageDataWidget* pageDataWidget);
	void goTo(PageDataWidget::PageDataType pageDataType);

protected:
	Q_SLOT void exportFilterData();

protected:
	PageDataWidget* m_pageDataWidget = nullptr;
};

}