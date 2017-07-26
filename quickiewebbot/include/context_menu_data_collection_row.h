#pragma once

namespace QuickieWebBot
{
	
class GridView;

class ContextMenuDataCollectionRow : public QMenu
{
	Q_OBJECT

public:
	ContextMenuDataCollectionRow(const QAbstractItemView* associatedAbstractView, QWidget* parent = nullptr);

private slots:
	void openUrlAction();

private:
	const QAbstractItemView* m_associatedAbstractView;

	QAction* m_openUrlAction;
	QAction* m_copyAction;
	QAction* m_refreshAllResultsAction;
	QAction* m_removeRowAction;
	QAction* m_checkIndexAction;
	QAction* m_checkGoogleCacheAction;
	QAction* m_openRobotsTxtAction;
	QAction* m_showOtherDomainsOnIpAction;
	QAction* m_checkHtmlAction;
};

}
