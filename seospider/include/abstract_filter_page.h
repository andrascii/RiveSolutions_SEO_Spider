#pragma once

#include "abstract_table_page.h"
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
class LookupLineEditWidget;
class ColumnsLookupLineEditWidget;
class ResourceTypeFilterWidget;

class AbstractFilterPage : public QFrame, public AbstractTablePage
{
	Q_OBJECT

public:
	AbstractFilterPage(WebSiteDataWidget* webSiteDataWidget, QWidget* parent = nullptr);

	void setSummaryViewDataAccessorType(SummaryDataAccessorFactory::DataAccessorType dataAccessorType);
	void selectFilter(CrawlerEngine::StorageType type) const;
	void selectParsedPage(int row);
	void selectTab(int pageDataType);
	void setInfoCategory(StorageAdapterType category, const QString& customDataFeed);
	const SummaryModel* summaryFilterModel() const;
	WebSiteDataWidget* websiteDataWidget();

	void enableSortableFilter();
	void enablePlainFilter();

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hasFilterSelection(int row) = 0;
	virtual void hasNoFilterSelection() = 0;
	virtual void initHeaderWidgets() final;

	virtual bool eventFilter(QObject* object, QEvent* event) override;

protected slots:
	void exportFilterData();
	void onApplyColumnSearch(int searchKey, const QString& searchValue);
	void onApplyPlainSearch(const QString& searchValue);
	void onResourceTypeFilterChanged(int filter);

private slots:
	void adjustSize();
	void reinitFilterTableSpans();
	void onSummaryViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	virtual QString pageName() const = 0;
	void applySearchHelper(int searchColumnNumber, const QString& searchValue);
	void prepareColumnSearchWidget(int row);
	void preparePlainSearchWidget(int row);
	void updateResourceTypeFilterWidget();
	void setCurrentStorageDescriptors(int row, CrawlerEngine::StorageType storageType);
	void resetCurrentStorageDescriptors();

private:
	struct SearchRules
	{
		int searchKey;
		QString searchData;
	};

	WebSiteDataWidget* m_webSiteDataWidget;

	TableView* m_summaryFilterTableView;
	SummaryModel* m_summaryFilterModel;
	SummaryViewModel* m_summaryFilterViewModel;

	QSplitter* m_splitter;
	FilterInfoWidget* m_info;

	// we store here current selected row and storage type
	// only for performance needs (see: onSummaryViewSelectionChanged)
	int m_currentSelectedRow;
	CrawlerEngine::StorageType m_currentStorageType;

	bool m_isFirstShow;

	ColumnsLookupLineEditWidget* m_columnsLookupLineEditWidget;
	LookupLineEditWidget* m_lookupLineEditWidget;
	QMap<int, SearchRules> m_searchRules;
	ResourceTypeFilterWidget* m_resourceTypeFilterWidget;
};

}