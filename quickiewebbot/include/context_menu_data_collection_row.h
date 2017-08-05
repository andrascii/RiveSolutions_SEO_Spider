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
	enum ActionType
	{
		OpenUrlActionType,
		RefreshAllResultsActionType,
		RemoveRowActionType,
		CheckIndexActionType,
		CheckGoogleActionType,
		OpenRobotsTxtActionType,
		ShowOtherDomainsOnIpActionType,
		CheckHtmlActionType,
		CopyToClipboardAllColumnsActionType,
		CopyToClipboardAllPagesActionType,
		CopyToClipboardUrlActionType
	};

	QModelIndexList selectedIndexes() const noexcept;

private slots:
	void openUrlAction();
	void copyToClipboardAllColumnsData();
	void copyToClipboardAllPages();
	void copyToClipboardUrl();

private:
	const GridView* m_associatedGridView;
	QMap<int, QAction*> m_actions;
};

}
