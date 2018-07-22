#include "all_resources_page.h"
#include "application.h"
#include "page_data_widget.h"
#include "website_data_widget.h"
#include "application.h"
#include "storage_exporter.h"
#include "data_collection_groups_factory.h"
#include "svg_renderer.h"
#include "summary_model.h"
#include "main_window.h"
#include "content_frame.h"

namespace SeoSpider
{

AllResourcesPage::AllResourcesPage(QWidget* parent)
	: AbstractFilterPage(new WebSiteDataWidget(nullptr), parent)
	, m_exportFilterDataAction(nullptr)
{
	PageDataWidget* resourceTables = new PageDataWidget(this);
	resourceTables->setPageDataType(PageDataWidget::LinksOnThisPageType);
	resourceTables->setPageDataType(PageDataWidget::LinksToThisPageType);
	resourceTables->setPageDataType(PageDataWidget::ServerResponseForPageType);

	websiteDataWidget()->setPageDataWidget(resourceTables);
	setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType::AllResourcesPage);

	createHeaderActionWidgets();
}

QWidget* AllResourcesPage::widget() const
{
	return const_cast<AllResourcesPage*>(this);
}

QString AllResourcesPage::name() const
{
	return tr("All Resources");
}

QIcon AllResourcesPage::icon() const
{
	QIcon icon;
	icon.addFile(QStringLiteral(":/images/all-resources-active.png"), QSize(), QIcon::Normal, QIcon::On);
	icon.addFile(QStringLiteral(":/images/all-resources-active.png"), QSize(), QIcon::Active, QIcon::On);
	icon.addFile(QStringLiteral(":/images/all-resources-active.png"), QSize(), QIcon::Active, QIcon::Off);
	icon.addFile(QStringLiteral(":/images/all-resources-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

	return icon;
}

IPage::Type AllResourcesPage::type() const
{
	return IPage::AllResourcesPage;
}

void AllResourcesPage::hasFilterSelection(int row)
{
	const DCStorageDescription* storageDescription = summaryFilterModel()->dataAccessor()->storageDescriptionByRow(row);
	m_exportFilterDataAction->setData(QVariant::fromValue(*storageDescription));
	m_exportFilterDataAction->setEnabled(true);
}

void AllResourcesPage::hasNoFilterSelection()
{
	m_exportFilterDataAction->setEnabled(false);
}

void AllResourcesPage::createHeaderActionWidgets()
{
	m_exportFilterDataAction = new QAction(SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20),
		tr("Export selected filter data to .xlsx file"), this);

	m_exportFilterDataAction->setDisabled(true);

	AbstractPage::addAction(m_exportFilterDataAction);

	VERIFY(connect(m_exportFilterDataAction, &QAction::triggered, this, &AllResourcesPage::exportFilterData));
}

}