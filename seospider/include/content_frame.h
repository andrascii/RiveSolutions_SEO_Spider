#pragma once

#include "ipage.h"

namespace SeoSpider
{

class HeaderDecorationWidget;

class ContentFrame : public QFrame
{
	Q_OBJECT

public:
	ContentFrame(QWidget* parent = nullptr);

	void addPage(IPage* page, bool setSelected = false);
	IPage* page(IPage::Type pageType) const;
	IPage* activePage() const;
	QWidget* pageWidget(IPage::Type pageType) const;

public slots:
	void showPage(IPage::Type pageType);

private slots:
	void handleNavigationPanelButtonClick();
	void onStateChanged(int state);
	void onDynamicControlsChanged(IPage::Type pageType);

private:
	void initializeNavigationPanelWidget();

private:
	struct NavigationPanelControls
	{
		QWidget* navigationPanelWidget;

		std::map<IPage::Type, QPushButton*> pushButtons;
	};

	QStackedWidget* m_stackedWidget;
	QSplitter* m_splitter;
	QMap<IPage::Type, int> m_pageIndexes;
	NavigationPanelControls m_navigationPanel;
	QPushButton* m_prevButton;
	HeaderDecorationWidget* m_decorationWidget;
	IPage::Type m_activePage;
	QHBoxLayout* m_dynamicControlsLayout;
};

}
