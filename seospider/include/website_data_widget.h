#pragma once

#include "storage_adaptor_type.h"

namespace SeoSpider
{

class TableView;
class PageModel;
class PageViewModel;
class IStorageAdaptor;
class PageDataWidget;

class WebSiteDataWidget : public QFrame
{
	Q_OBJECT

public:
	WebSiteDataWidget(PageDataWidget* pageDataWidget = nullptr, QWidget* parent = nullptr);

	void setStorageAdaptorType(StorageAdaptorType storageAdaptorType);

protected:
	Q_SLOT void pageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	virtual void showEvent(QShowEvent* event) override;

private:
	QSplitter* m_splitter;

	QStackedWidget* m_stackedWidget;

	QMap<StorageAdaptorType, int> m_tables;

	PageDataWidget* m_pageDataWidget;
};

}