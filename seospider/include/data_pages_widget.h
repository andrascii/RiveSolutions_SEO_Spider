#pragma once

namespace SeoSpider
{

class SummaryFilterWidget;

class DataPagesWidget : public QFrame
{
	Q_OBJECT

public:
	enum class Page
	{
		SiteStructurePanelPage,
		SiteAuditPage,
		AllPagesPage,
		AllResourcesPage,
		DomainMetricsPage,
		ReportsPage
	};

	DataPagesWidget(QWidget* parent = nullptr);

	Q_SLOT void showPage(Page page);

private:
	Q_SLOT void handleNavigationPanelButtonClick();

private:
	void initializeNavigationPanelWidget();
	void initializeStackedWidget();

private:
	struct NavigationPanelControls
	{
		QWidget* navigationPanelWidget;
		QVBoxLayout* verticalMainLayout;
		std::map<Page, QPushButton*> pushButtons;
	};

	QStackedWidget* m_stackedWidget;
	std::map<Page, int> m_pageIndexes;

	NavigationPanelControls m_navigationPanel;
	QPushButton* m_prevButton;
};

}