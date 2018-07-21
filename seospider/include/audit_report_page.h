#pragma once

#include "abstract_page.h"

namespace SeoSpider
{

class ReportsPage;

class AuditReportPage : public QFrame, public AbstractPage
{
	Q_OBJECT

public:
	AuditReportPage(QWidget* parent = nullptr);
	virtual QWidget* widget() const override;
	virtual QString name() const override;
	virtual QIcon icon() const override;
	virtual Type type() const override;

signals:
	virtual void controlsChanged(IPage::Type) override;

private:
	void createHeaderActionWidgets();

private:
	ReportsPage* m_reportsPage;
};

}