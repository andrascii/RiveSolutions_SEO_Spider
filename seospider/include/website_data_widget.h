#pragma once

#include "storage_adapter_type.h"

namespace SeoSpider
{

class TableView;
class PageModel;
class PageViewModel;
class IStorageAdapter;
class PageDataWidget;
class PageDataWidgetSplitter;

class WebSiteDataWidget : public QFrame
{
	Q_OBJECT

public:
	WebSiteDataWidget(PageDataWidget* pageDataWidget = nullptr, QWidget* parent = nullptr);

	void setStorageAdapterType(StorageAdapterType storageAdapterType);

	void setPageDataWidget(PageDataWidget* dataWidget);
	PageDataWidget* pageDataWidget() const noexcept;

	void selectParsedPage(int row) const noexcept;

private slots:
	void pageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	PageDataWidgetSplitter* m_splitter;

	QStackedWidget* m_stackedWidget;

	QMap<StorageAdapterType, int> m_tables;

	PageDataWidget* m_pageDataWidget;
};

}