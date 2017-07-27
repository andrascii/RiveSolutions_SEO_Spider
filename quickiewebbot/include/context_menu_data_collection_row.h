#pragma once

#include "page_info.h"

namespace QuickieWebBot
{
	
class GridView;

class ContextMenuDataCollectionRow : public QMenu
{
	Q_OBJECT

public:
	ContextMenuDataCollectionRow(const GridView* associatedGridView, QWidget* parent = nullptr);
	
private:
	PageInfoPtr pageInfoAtPopuppedPoint() const noexcept;
	QModelIndex popuppedPointModelIndex() const noexcept;
	QModelIndexList selectedRowsList() const noexcept;
	GridViewModel* model() const noexcept;

private slots:
	void openUrlAction();
	void copyToClipboardAllColumnsData();
	void copyToClipboardAllPages();
	void copyToClipboardUrl();

private:
	const GridView* m_associatedGridView;

	QAction* m_openUrlAction;
	QAction* m_refreshAllResultsAction;
	QAction* m_removeRowAction;
	QAction* m_checkIndexAction;
	QAction* m_checkGoogleCacheAction;
	QAction* m_openRobotsTxtAction;
	QAction* m_showOtherDomainsOnIpAction;
	QAction* m_checkHtmlAction;

	QAction* m_copyToClipboardAllColumnsAction;
	QAction* m_copyToClipboardAllPagesAction;
	QAction* m_copyToClipboardUrlAction;

};

}
