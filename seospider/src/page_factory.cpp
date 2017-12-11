#include "page_factory.h"
#include "filter_widget.h"
#include "website_data_widget.h"
#include "page_data_widget.h"
#include "page_model.h"
#include "page_view_model.h"
#include "table_view.h"
#include "context_menu_data_collection_row.h"
#include "application.h"

namespace SeoSpider
{

QWidget* PageFactory::createPage(Page page) const
{
	DEBUG_ASSERT(page >= SiteAuditPage && page <= ReportsPage);

	QWidget* widget = nullptr;

	switch (page)
	{
		case SiteAuditPage:
		{
			FilterWidget* siteAuditPage = new FilterWidget(new WebSiteDataWidget(nullptr));
			siteAuditPage->setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType::ErrorsFilterPage);

			widget = siteAuditPage;

			break;
		}
		case AllPagesPage:
		{
			TableView* crawlingTableView = new TableView;
			PageModel* model = new PageModel;
			PageViewModel* modelView = new PageViewModel(crawlingTableView, model);

			model->setStorageAdapter(theApp->storageAdapterFactory()->createParsedPageInfoStorage(StorageAdapterType::StorageAdapterTypeAllPages, theApp->sequencedDataCollection()));
			crawlingTableView->setModel(model);
			crawlingTableView->setViewModel(modelView);
			crawlingTableView->setContextMenu(new ContextMenuDataCollectionRow(crawlingTableView));
			crawlingTableView->setShowAdditionalGrid(true);

			widget = crawlingTableView;

			break;
		}
		case AllResourcesPage:
		{
			PageDataWidget* resourceTables = new PageDataWidget;
			resourceTables->setPageDataType(PageDataWidget::LinksOnThisPageType);
			resourceTables->setPageDataType(PageDataWidget::LinksToThisPageType);
			resourceTables->setPageDataType(PageDataWidget::ServerResponseForPageType);

			FilterWidget* allResourcesPage = new FilterWidget(new WebSiteDataWidget(resourceTables));
			allResourcesPage->setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType::AllResourcesPage);

			widget = allResourcesPage;

			break;
		}
		case DomainMetricsPage:
		{
			
		}
		case ReportsPage:
		{

		}
	}

	return widget;
}

QIcon PageFactory::createPageIcon(Page page) const
{
	QIcon icon;

	switch (page)
	{
		case SiteAuditPage:
		{
			icon.addFile(QStringLiteral(":/images/audit-info-active.png"), QSize(), QIcon::Normal, QIcon::On);
			icon.addFile(QStringLiteral(":/images/audit-info-active.png"), QSize(), QIcon::Active, QIcon::On);
			icon.addFile(QStringLiteral(":/images/audit-info-active.png"), QSize(), QIcon::Active, QIcon::Off);
			icon.addFile(QStringLiteral(":/images/audit-info-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

			break;
		}
		case AllPagesPage:
		{
			icon.addFile(QStringLiteral(":/images/all-pages-active.png"), QSize(), QIcon::Normal, QIcon::On);
			icon.addFile(QStringLiteral(":/images/all-pages-active.png"), QSize(), QIcon::Active, QIcon::On);
			icon.addFile(QStringLiteral(":/images/all-pages-active.png"), QSize(), QIcon::Active, QIcon::Off);
			icon.addFile(QStringLiteral(":/images/all-pages-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

			break;
		}
		case AllResourcesPage:
		{
			icon.addFile(QStringLiteral(":/images/all-resources-active.png"), QSize(), QIcon::Normal, QIcon::On);
			icon.addFile(QStringLiteral(":/images/all-resources-active.png"), QSize(), QIcon::Active, QIcon::On);
			icon.addFile(QStringLiteral(":/images/all-resources-active.png"), QSize(), QIcon::Active, QIcon::Off);
			icon.addFile(QStringLiteral(":/images/all-resources-normal.png"), QSize(), QIcon::Normal, QIcon::Off);

			break;
		}
		case DomainMetricsPage:
		{

		}
		case ReportsPage:
		{

		}
	}

	return icon;
}

}