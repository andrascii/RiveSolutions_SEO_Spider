#pragma once

namespace QuickieWebBot
{

class TableView;


class AllResourcesWidget : public QFrame
{
	Q_OBJECT

public:
	AllResourcesWidget(QWidget* parent = nullptr);

private:
	void initializeResourcesTableView();
	void initializewebResourcePagesTable();

	Q_SLOT void onFilterViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	Q_SLOT void onPageViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	void setPageServerResponse(const PageRawInfoPtr& page);

private:
	TableView* m_resourcesTableView;
	TableView* m_webResourcePagesTable;
	TableView* m_linksFromThisPage;
	TableView* m_linksToThisPage;
	QLabel* m_httpResponse;
};

}