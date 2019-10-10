#pragma once
#include "page_data_widget.h"

namespace SeoSpider
{

class IPage
{
public:
	enum Type
	{
		SiteAuditPage,
		AllPagesPage,
		AllResourcesPage,
		DomainMetricsPage,
		AuditReportPage
	};

	virtual ~IPage() = default;
	virtual QWidget* widget() const = 0;
	virtual QList<QWidget*> bindControls() const = 0;
	virtual void addWidget(QWidget* control, int position = -1) = 0;
	virtual void addAction(QAction* action, int position = -1) = 0;
	virtual QString name() const = 0;
	virtual QIcon icon() const = 0;
	virtual Type type() const = 0;
	virtual void showMe() const = 0;

	// signals
	virtual void controlsChanged(IPage::Type) = 0;
};

}
