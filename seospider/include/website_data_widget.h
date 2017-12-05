#pragma once

#include "storage_adapter_type.h"

namespace SeoSpider
{

class TableView;
class PageModel;
class PageViewModel;
class IStorageAdapter;
class PageDataWidget;

class WebSiteDataWidget : public QFrame
{
	Q_OBJECT

public:
	WebSiteDataWidget(PageDataWidget* pageDataWidget = nullptr, QWidget* parent = nullptr);

	void setStorageAdaptorType(StorageAdapterType storageAdaptorType);

protected:
	virtual void showEvent(QShowEvent* event) override;

private slots:
	void pageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	QSplitter* m_splitter;

	QStackedWidget* m_stackedWidget;

	QMap<StorageAdapterType, int> m_tables;

	PageDataWidget* m_pageDataWidget;

	bool m_wasShown;
};

}