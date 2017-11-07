#pragma once

#include "summary_data_accessor_factory.h"

namespace SeoSpider
{

class WebSiteDataWidget;
class TableView;
class SummaryModel;
class SummaryViewModel;
class AbstractWebSiteDataWidget;

class FilterWidget : public QFrame
{
	Q_OBJECT

public:
	FilterWidget(WebSiteDataWidget* webSiteDataWidget, QWidget* parent = nullptr);

	void setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType) const;

protected:
	virtual void showEvent(QShowEvent* event) override;

	void adjustSize();

private:
	Q_SLOT void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) const;

private:
	WebSiteDataWidget* m_webSiteDataWidget;

	TableView* m_summaryFilterTableView;
	SummaryModel* m_summaryFilterModel;
	SummaryViewModel* m_summaryFilterViewModel;

	QSplitter* m_splitter;

	bool m_isFirstShow;
};

}