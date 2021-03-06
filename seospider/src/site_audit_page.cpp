#include "stdafx.h"
#include "site_audit_page.h"
#include "website_data_widget.h"
#include "svg_renderer.h"
#include "header_toolbutton_creator.h"
#include "data_collection_groups_factory.h"
#include "application.h"
#include "storage_exporter.h"
#include "summary_model.h"
#include "page_data_widget.h"
#include "columns_lookup_lineedit_widget.h"

namespace SeoSpider
{

SiteAuditPage::SiteAuditPage(QWidget* parent)
	: AbstractFilterPage(new WebSiteDataWidget(nullptr), parent)
	, m_exportFilterDataAction(nullptr)
	, m_switchAuditInfoFilterWidgetGroupingAction(nullptr)
{
	m_pageDataWidget = new PageDataWidget(this);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksOnThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksToThisPageType);
	m_pageDataWidget->setPageDataType(PageDataWidget::LinksWithSameCanonicalURLType);
	m_pageDataWidget->setPageDataType(PageDataWidget::ServerResponseForPageType);

	websiteDataWidget()->setPageDataWidget(m_pageDataWidget);

	setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType::ErrorsFilterPage);
	createHeaderActionWidgets();
}

QWidget* SiteAuditPage::widget() const
{
	return const_cast<SiteAuditPage*>(this);
}

QString SiteAuditPage::name() const
{
	return tr("Audit Info");
}

QIcon SiteAuditPage::icon() const
{
	QIcon icon;
	icon.addPixmap(SvgRenderer::render(":/images/audit-info-page-icon.svg", 15, 15), QIcon::Normal, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/audit-info-page-icon.svg", 15, 15), QIcon::Active, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/audit-info-page-icon.svg", 15, 15), QIcon::Active, QIcon::Off);
	icon.addPixmap(SvgRenderer::render(":/images/audit-info-page-icon.svg", 15, 15), QIcon::Normal, QIcon::Off);

	return icon;
}

IPage::Type SiteAuditPage::type() const
{
	return IPage::SiteAuditPage;
}

void SiteAuditPage::hasFilterSelection(int row)
{
	const DCStorageDescription* storageDescription = summaryFilterModel()->dataAccessor()->storageDescriptionByRow(row);
	m_exportFilterDataAction->setData(QVariant::fromValue(*storageDescription));
	m_exportFilterDataAction->setEnabled(true);
}

void SiteAuditPage::hasNoFilterSelection()
{
	m_exportFilterDataAction->setEnabled(false);
}

void SiteAuditPage::createHeaderActionWidgets()
{
	AbstractFilterPage::initHeaderWidgets();

	m_switchAuditInfoFilterWidgetGroupingAction = new QAction(
		SvgRenderer::render(QStringLiteral(":/images/group-by-category.svg"), 20, 20),
		tr("Change filters grouping"), this);

	QToolButton* menuButton = qobject_cast<QToolButton*>(createControl(m_switchAuditInfoFilterWidgetGroupingAction));

	ASSERT(menuButton);

	QMenu* groupingMenu = new QMenu(menuButton);

	QAction* groupByCategoryAction = new QAction(SvgRenderer::render(":/images/group-by-category.svg", 20, 20), tr("Group filters by category"));
	QAction* groupByErrorLevelAction = new QAction(SvgRenderer::render(":/images/group-by-level.svg", 20, 20), tr("Group filters by error level"));
	groupByErrorLevelAction->setData(true);
	groupByCategoryAction->setData(false);

	groupingMenu->addAction(groupByCategoryAction);
	groupingMenu->addAction(groupByErrorLevelAction);

	menuButton->setMenu(groupingMenu);
	menuButton->setPopupMode(QToolButton::InstantPopup);

	AbstractPage::addWidget(menuButton);

	VERIFY(connect(groupingMenu, SIGNAL(triggered(QAction*)), this, SLOT(onChangeGroupingAuditInfo(QAction*))));

	//////////////////////////////////////////////////////////////////////////

	m_exportFilterDataAction = new QAction(SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20),
		tr("Export selected filter data to .xlsx file"), this);

	m_exportFilterDataAction->setDisabled(true);

	AbstractPage::addAction(m_exportFilterDataAction);

	VERIFY(connect(m_exportFilterDataAction, &QAction::triggered, this, &SiteAuditPage::exportFilterData));
}

QString SiteAuditPage::pageName() const
{
	return QStringLiteral("SiteAuditPage");
}

void SiteAuditPage::onChangeGroupingAuditInfo(QAction* action)
{
	m_switchAuditInfoFilterWidgetGroupingAction->setIcon(action->icon());

	showMe();

	ASSERT(action->data().type() == QVariant::Bool);
	bool isSortingEnabled = action->data().toBool();

	if (isSortingEnabled)
	{
		enableSortableFilter();
	}
	else
	{
		enablePlainFilter();
	}
}

}
