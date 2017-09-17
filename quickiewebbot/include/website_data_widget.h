#pragma once

#include "storage_adaptor_type.h"

namespace QuickieWebBot
{

class TableView;
class PageModel;
class PageViewModel;
class IStorageAdaptor;

class WebSiteDataWidget : public QFrame
{
	Q_OBJECT

public:
	WebSiteDataWidget(QWidget* parent = nullptr);

	void setStorageAdaptorType(StorageAdaptorType storageAdaptorType);

private:
	QStackedWidget* m_stackedWidget;
	QMap<StorageAdaptorType, int> m_tables;
};

}