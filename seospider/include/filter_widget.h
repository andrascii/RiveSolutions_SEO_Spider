#pragma once

#include "summary_data_accessor_factory.h"
#include "storage_type.h"
#include "filter_info_factory.h"

namespace SeoSpider
{

class WebSiteDataWidget;
class TableView;
class SummaryModel;
class SummaryViewModel;
class AbstractWebSiteDataWidget;

class FilterInfoWidget : public QFrame
{
	Q_OBJECT
public:
	FilterInfoWidget(QWidget* parent);

	void setFilterInfo(const FilterInfo& filterInfo);

private slots:
	void onPropertyChanged();

private:
	QLabel* m_title;
	QLabel* m_description;
	FilterInfo m_filterInfo;
};

class FilterWidget : public QFrame
{
	Q_OBJECT

public:
	FilterWidget(WebSiteDataWidget* webSiteDataWidget, QWidget* parent = nullptr);

	void setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType) const;
	void selectFilter(CrawlerEngine::StorageType type) const;
	void selectParsedPage(int row);
	void selectTab(int pageDataType);

private:
	Q_SLOT void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	Q_SLOT void adjustSize();

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
