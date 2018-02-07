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
	QWidget* page(PageFactory::Page page) const noexcept;

public slots:
	void showPage(PageFactory::Page page);

private slots:
	void handleNavigationPanelButtonClick();
	void onStateChanged(int state);
	void onDynamicControlsChanged(int page);

private:
	void initializeNavigationPanelWidget();

private:
	struct NavigationPanelControls
	{
		QWidget* navigationPanelWidget;

		std::map<PageFactory::Page, QPushButton*> pushButtons;
	};

	QStackedWidget* m_stackedWidget;
	
	QMap<PageFactory::Page, int> m_pageIndexes;

	NavigationPanelControls m_navigationPanel;

	QPushButton* m_prevButton;

	HeaderDecorationWidget* m_decorationWidget;

	PageFactory::Page m_activePage;

	QHBoxLayout* m_dynamicControlsLayout;
};

}
