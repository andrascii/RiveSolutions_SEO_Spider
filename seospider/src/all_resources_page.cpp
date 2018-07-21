#include "all_resources_page.h"
#include "application.h"
#include "page_data_widget.h"
#include "website_data_widget.h"
#include "action_registry.h"
#include "action_keys.h"
#include "application.h"
#include "crawler.h"
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
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	const DCStorageDescription* storageDescription = summaryFilterModel()->dataAccessor()->storageDescriptionByRow(row);
	actionRegistry.globalAction(s_exportFilterDataAllResourcesPageAction)->setData(QVariant::fromValue(*storageDescription));
	actionRegistry.globalAction(s_exportFilterDataAllResourcesPageAction)->setEnabled(true);
}

void AllResourcesPage::hasNoFilterSelection()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	actionRegistry.globalAction(s_exportFilterDataAllResourcesPageAction)->setEnabled(false);
}

void AllResourcesPage::createHeaderActionWidgets()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	// export actions
	QAction* exportFilterDataAllResourcesAction = actionRegistry.addGlobalAction(s_exportFilterDataAllResourcesPageAction,
		SvgRenderer::render(QStringLiteral(":/images/excel.svg"), 20, 20), tr("Export selected filter data to .xlsx file"));

	exportFilterDataAllResourcesAction->setDisabled(true);

	AbstractPage::addAction(exportFilterDataAllResourcesAction);

	VERIFY(connect(exportFilterDataAllResourcesAction, &QAction::triggered, this, &AllResourcesPage::exportFilterData));
}

void AllResourcesPage::exportFilterData()
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