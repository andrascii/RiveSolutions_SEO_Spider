#pragma once

namespace SeoSpider
{

class PageFactory
{
public:
	enum Page
	{
		SiteAuditPage,
		AllPagesPage,
		AllResourcesPage,
		DomainMetricsPage,
		ReportsPage
	};

	QWidget* createPage(Page page) const;
};

}