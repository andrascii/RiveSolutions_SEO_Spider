#pragma once

#include "abstract_page.h"
#include "summary_data_accessor_factory.h"
#include "storage_type.h"
#include "filter_info_factory.h"

namespace SeoSpider
{

class WebSiteDataWidget;
class TableView;
class SummaryModel;
class SummaryViewModel;
class FilterInfoWidget;

class AbstractFilterPage : public QFrame, public AbstractPage
{
	Q_OBJECT

public:
	AbstractFilterPage(WebSiteDataWidget* webSiteDataWidget, QWidget* parent = nullptr);

	void setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType);
	void selectFilter(CrawlerEngine::StorageType type) const;
	void selectParsedPage(int row);
	void selectTab(int pageDataType);
	void setInfoCategory(StorageAdapterType category);
	const SummaryModel* summaryFilterModel() const;
	WebSiteDataWidget* websiteDataWidget();

	void enableSortableFilter();
	void enablePlainFilter();

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hasFilterSelection(int row) = 0;
	virtual void hasNoFilterSelection() = 0;

protected slots:
	void exportFilterData();

private slots:
	void adjustSize();
	void reinitFilterTableSpans();
	void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	WebSiteDataWidget* m_webSiteDataWidget;

	TableView* m_summaryFilterTableView;
	SummaryModel* m_summaryFilterModel;
	SummaryViewModel* m_summaryFilterViewModel;

	QSplitter* m_splitter;
	FilterInfoWidget* m_info;

	bool m_isFirstShow;
};

}