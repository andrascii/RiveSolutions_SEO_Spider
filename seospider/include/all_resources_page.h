#pragma once

#include "abstract_filter_page.h"
#include "storage_type.h"

namespace SeoSpider
{

class AllResourcesPage : public AbstractFilterPage
{
	Q_OBJECT

public:
	AllResourcesPage(QWidget* parent = nullptr);
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
	void exportFilterData();
};

}