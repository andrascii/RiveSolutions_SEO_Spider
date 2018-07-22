#pragma once

#include "abstract_filter_page.h"

namespace SeoSpider
{

class FilterWidget;

class SiteAuditPage : public AbstractFilterPage
{
	Q_OBJECT

public:
	SiteAuditPage(QWidget* parent = nullptr);
	virtual QWidget* widget() const override;
	virtual QString name() const override;
	virtual QIcon icon() const override;
	virtual Type type() const override;

signals:
	virtual void controlsChanged(IPage::Type) override;

protected:
	virtual void hasFilterSelection(int row) override;
	virtual void hasNoFilterSelection() override;

private:
	void createHeaderActionWidgets();

private slots:
	void onChangeGroupingAuditInfo(QAction* action);

private:
	QAction* m_exportFilterDataAction;
	QAction* m_switchAuditInfoFilterWidgetGroupingAction;
};

}