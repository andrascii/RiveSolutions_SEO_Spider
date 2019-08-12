#pragma once
#include "stdafx.h"
#include "reports_page.h"

namespace SeoSpider
{

class ReportDataProvider;
	
class IReportExporter
{
public:
	virtual ~IReportExporter() = default;
	virtual QString description() const = 0;
	virtual QString fileMask() const = 0;
	virtual QString ext() const = 0;

	virtual void doExport(QIODevice* device, ReportsPage::ReportType reportType, 
		const ReportDataProvider* provider) const = 0;
};

}
