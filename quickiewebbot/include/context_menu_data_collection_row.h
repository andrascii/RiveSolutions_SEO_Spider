#pragma once

#include "parsed_page_info.h"

namespace QuickieWebBot
{
	
class TableView;

class ContextMenuDataCollectionRow : public QMenu
{
	Q_OBJECT

public:
	ContextMenuDataCollectionRow(const TableView* associatedGridView, QWidget* parent = nullptr);
	
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
	const TableView* m_associatedView;
	QMap<int, QAction*> m_actions;
};

}
