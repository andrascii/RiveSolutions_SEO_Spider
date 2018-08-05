#pragma once

#include "abstract_table_page.h"

namespace SeoSpider
{

AbstractTablePage::AbstractTablePage(PageDataWidget* pageDataWidget)
	: m_pageDataWidget(pageDataWidget)
{
	// ASSERT(m_pageDataWidget != nullptr);
}

void AbstractTablePage::goTo(PageDataWidget::PageDataType pageDataType)
{
	if (m_pageDataWidget != nullptr)
	{
		m_pageDataWidget->selectTab(pageDataType);
	}
}

}