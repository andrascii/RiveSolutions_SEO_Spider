#include "stdafx.h"
#include "all_resources_page.h"
#include "page_data_widget.h"
#include "website_data_widget.h"
#include "data_collection_groups_factory.h"
#include "svg_renderer.h"
#include "summary_model.h"
#include "content_frame.h"

namespace SeoSpider
{

AllResourcesPage::AllResourcesPage(QWidget* parent)
	: AbstractFilterPage(new WebSiteDataWidget(nullptr), parent)
	, m_exportFilterDataAction(nullptr)
{
	m_pageDataWidget = new PageDataWidget(this);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksOnThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksToThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksWithSameCanonicalURLType);
	m_pageDataWidget->setPageDataType(PageDataWidget::ServerResponseForPageType);

	websiteDataWidget()->setPageDataWidget(m_pageDataWidget);
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
	icon.addPixmap(SvgRenderer::render(":/images/all-resources-page-icon.svg", 15, 15), QIcon::Normal, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/all-resources-page-icon.svg", 15, 15), QIcon::Active, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/all-resources-page-icon.svg", 15, 15), QIcon::Active, QIcon::Off);
	icon.addPixmap(SvgRenderer::render(":/images/all-resources-page-icon.svg", 15, 15), QIcon::Normal, QIcon::Off);

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
	AbstractFilterPage::initHeaderWidgets();

	m_exportFilterDataAction = new QAction(SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20),
		tr("Export selected filter data to .xlsx file"), this);

	m_exportFilterDataAction->setDisabled(true);

	AbstractPage::addAction(m_exportFilterDataAction);

	VERIFY(connect(m_exportFilterDataAction, &QAction::triggered, this, &AllResourcesPage::exportFilterData));
}

QString AllResourcesPage::pageName() const
{
	return QStringLiteral("AllResourcesPage");
}

}
