#pragma once

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
	void openUrlAction() const;
	void copyToClipboardAllColumnsData() const;
	void copyToClipboardAllPages() const;
	void copyToClipboardUrl() const;

private:
	const TableView* m_associatedView;
	QMap<int, QAction*> m_actions;
};

}
