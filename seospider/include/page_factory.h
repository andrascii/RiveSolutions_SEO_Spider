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
		AuditReportPage
	};

	QWidget* createPage(Page page) const;

	QIcon createPageIcon(Page page) const;
};

}