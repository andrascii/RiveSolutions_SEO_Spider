#pragma once

#include "ui_navigation_panel.h"

namespace QuickieWebBot
{

class NavigationPanelWidget 
	: public QWidget
	, public Ui::NavigationPanel
{
public:
	NavigationPanelWidget(QWidget* parent);

protected:
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void init();
	void initCrawlingPage();

private:
	QPushButton* m_prevSelectedButton;
};

}