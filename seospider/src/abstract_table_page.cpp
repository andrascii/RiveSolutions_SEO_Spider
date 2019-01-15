#pragma once

#include "abstract_table_page.h"
#include "dc_storage_group_description.h"
#include "storage_exporter.h"
#include "application.h"
#include "crawler.h"

namespace SeoSpider
{

AbstractTablePage::AbstractTablePage(QWidget* parent, PageDataWidget* pageDataWidget)
	: QFrame(parent)
	, m_pageDataWidget(pageDataWidget)
{
	// ASSERT(m_pageDataWidget != nullptr);
	if (m_pageDataWidget != nullptr)
	{
		pageDataWidget->setParent(this);
	}
}

void AbstractTablePage::goTo(PageDataWidget::PageDataType pageDataType)
{
	if (m_pageDataWidget != nullptr)
	{
		m_pageDataWidget->selectTab(pageDataType);
	}
}

void AbstractTablePage::exportFilterData()
{
	QAction* action = qobject_cast<QAction*>(sender());
	ASSERT(action && "This method must be called using QAction");

	const QVariant objectData = action->data();
	ASSERT(objectData.isValid() && "No data passed");

	std::vector<DCStorageDescription> storages;
	storages.push_back(qvariant_cast<DCStorageDescription>(objectData));

	StorageExporter::exportStorage(theApp->crawler()->sequencedDataCollection(), storages);
}

}
