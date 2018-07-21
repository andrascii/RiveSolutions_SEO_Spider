#include "site_audit_page.h"
#include "website_data_widget.h"
#include "action_registry.h"
#include "action_keys.h"
#include "svg_renderer.h"
#include "header_toolbar_creator.h"
#include "data_collection_groups_factory.h"
#include "application.h"
#include "storage_exporter.h"
#include "crawler.h"
#include "summary_model.h"

namespace SeoSpider
{

SiteAuditPage::SiteAuditPage(QWidget* parent)
	: AbstractFilterPage(new WebSiteDataWidget(nullptr), parent)
{
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
	icon.addFile(QStringLiteral(":/images/audit-info-active.png"), QSize(), QIcon::Normal, QIcon::On);
	icon.addFile(QStringLiteral(":/images/audit-info-active.png"), QSize(), QIcon::Active, QIcon::On);
	icon.addFile(QStringLiteral(":/images/audit-info-active.png"), QSize(), QIcon::Active, QIcon::Off);
	icon.addFile(QStringLiteral(":/images/audit-info-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

	return icon;
}

IPage::Type SiteAuditPage::type() const
{
	return IPage::SiteAuditPage;
}

void SiteAuditPage::hasFilterSelection(int row)
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	const DCStorageDescription* storageDescription = summaryFilterModel()->dataAccessor()->storageDescriptionByRow(row);
	actionRegistry.globalAction(s_exportFilterDataAuditPageAction)->setData(QVariant::fromValue(*storageDescription));
	actionRegistry.globalAction(s_exportFilterDataAuditPageAction)->setEnabled(true);
}

void SiteAuditPage::hasNoFilterSelection()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	actionRegistry.globalAction(s_exportFilterDataAuditPageAction)->setEnabled(false);
}

void SiteAuditPage::createHeaderActionWidgets()
{
	// TODO: remove switchAuditInfoFilterWidgetGroupingAction from ActionRegistry
	// because this container must contain only common actions

	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// switch grouping action
	QAction* switchAuditInfoFilterWidgetGroupingAction = actionRegistry.addGlobalAction(s_switchAuditInfoFilterWidgetGroupingAction,
		SvgRenderer::render(QStringLiteral(":/images/group-by-category.svg"), 20, 20), tr("Change filters grouping"));

	QToolButton* menuButton = qobject_cast<QToolButton*>(HeaderToolButtonCreator::createControl(switchAuditInfoFilterWidgetGroupingAction));

	ASSERT(menuButton);

	QMenu* groupingMenu = new QMenu(menuButton);

	QAction* groupByCategoryAction = new QAction(SvgRenderer::render(":/images/group-by-category.svg", 20, 20), "Group filters by category");
	QAction* groupByErrorLevelAction = new QAction(SvgRenderer::render(":/images/group-by-level.svg", 20, 20), "Group filters by error level");
	groupByErrorLevelAction->setData(true);
	groupByCategoryAction->setData(false);

	groupingMenu->addAction(groupByCategoryAction);
	groupingMenu->addAction(groupByErrorLevelAction);

	menuButton->setMenu(groupingMenu);
	menuButton->setPopupMode(QToolButton::InstantPopup);

	AbstractPage::addWidget(menuButton);

	VERIFY(connect(groupingMenu, SIGNAL(triggered(QAction*)), this, SLOT(onChangeGroupingAuditInfo(QAction*))));

	//////////////////////////////////////////////////////////////////////////

	// export actions
	QAction* exportFilterDataAuditPageAction = actionRegistry.addGlobalAction(s_exportFilterDataAuditPageAction,
		SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20), tr("Export selected filter data to .xlsx file"));

	exportFilterDataAuditPageAction->setDisabled(true);

	AbstractPage::addAction(exportFilterDataAuditPageAction);

	VERIFY(connect(exportFilterDataAuditPageAction, &QAction::triggered, this, &SiteAuditPage::exportFilterData));
}

void SiteAuditPage::onChangeGroupingAuditInfo(QAction* action)
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	actionRegistry.globalAction(s_switchAuditInfoFilterWidgetGroupingAction)->setIcon(action->icon());

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

void SiteAuditPage::exportFilterData()
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