#pragma once

#include "page_factory.h"

namespace SeoSpider
{

class HeaderDecorationWidget;

class DataPagesWidget : public QFrame
{
	Q_OBJECT

public:
	DataPagesWidget(QWidget* parent = nullptr);

	void addPage(PageFactory::Page page, QWidget* widget, const QString& buttonText, const QIcon& buttonIcon = QIcon(), bool setSelected = false);

public slots:
	void showPage(PageFactory::Page page);

private slots:
	void handleNavigationPanelButtonClick();

private:
	void initializeNavigationPanelWidget();

private:
	struct NavigationPanelControls
	{
		QWidget* navigationPanelWidget;

		std::map<PageFactory::Page, QPushButton*> pushButtons;
	};

	QStackedWidget* m_stackedWidget;
	
	std::map<PageFactory::Page, int> m_pageIndexes;

	NavigationPanelControls m_navigationPanel;

	QPushButton* m_prevButton;

	HeaderDecorationWidget* m_decorationWidget;
};

}
