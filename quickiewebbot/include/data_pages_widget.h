#pragma once

#include "ui_data_pages.h"

namespace QuickieWebBot
{

class ErrorsFilterWidget;

class DataPagesWidget : public QWidget
{
	Q_OBJECT

public:
	enum class Page
	{
		SiteStructurePanelPage,
		SeoAnalysisPage,
		AllPagesPage,
		AllResourcesPage,
		DomainMetricsPage,
		ReportsPage
	};

	DataPagesWidget(QWidget* parent = nullptr);

	Q_SLOT void showPage(Page page);
	Q_SLOT void handleNavigationPanelButtonClick();

private:
	void initializeNavigationPanelWidget();
	void initializeStackedWidget();

private:
	struct NavigationPanelControls
	{
		QWidget* navigationPanelWidget;
		QVBoxLayout* verticalMainLayout;

		QWidget* siteStructurePanelWidget;
		QVBoxLayout* verticalSubbuttonsLayout;

		std::map<Page, QPushButton*> pushButtons;
	};

	QStackedWidget* m_stackedWidget;
	std::map<Page, int> m_pageIndexes;

	NavigationPanelControls m_navigationPanel;
};

}
