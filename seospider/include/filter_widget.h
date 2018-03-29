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

	void addSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType);
	void selectFilter(CrawlerEngine::StorageType type) const;
	void selectParsedPage(int row);
	void selectTab(int pageDataType);
	void switchFilterTo(SummaryDataAccessorFactory::DataAccessorType dataAccessorType);

private:
	void createSummaryFilterTable(SummaryDataAccessorFactory::DataAccessorType dataAccessorType);
	SummaryDataAccessorFactory::DataAccessorType currentKey() const;

	Q_SLOT void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	Q_SLOT void adjustSize();

private:
	WebSiteDataWidget* m_webSiteDataWidget;

	QMap<SummaryDataAccessorFactory::DataAccessorType, TableView*> m_summaryFilterTableViews;
	QMap<SummaryDataAccessorFactory::DataAccessorType, SummaryModel*> m_summaryFilterModels;
	QMap<SummaryDataAccessorFactory::DataAccessorType, SummaryViewModel*> m_summaryFilterViewModels;

	QStackedWidget* m_stackedFilterWidget;
	QMap<SummaryDataAccessorFactory::DataAccessorType, int> m_dataAccessorIndices;

	QSplitter* m_splitter;
	FilterInfoWidget* m_info;

	bool m_isFirstShow;
};

}
