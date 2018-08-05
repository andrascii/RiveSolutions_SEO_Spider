#pragma once

#include "abstract_page.h"

namespace SeoSpider
{
	
class AbstractTablePage: public AbstractPage
{
public:
	AbstractTablePage(PageDataWidget* pageDataWidget);
	void goTo(PageDataWidget::PageDataType pageDataType);

protected:
	PageDataWidget* m_pageDataWidget = nullptr;
};

}