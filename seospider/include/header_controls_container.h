#pragma once

#include "page_factory.h"

namespace SeoSpider
{

class HeaderControlsContainer: public QObject
{
	Q_OBJECT
public:
	HeaderControlsContainer();

	void addAction(QAction* action, PageFactory::Page page, int position = -1);
	void addWidget(QWidget* widget, PageFactory::Page page, int position = -1);

	QList<QWidget*> controls(PageFactory::Page page) const;

	void setActivePage(PageFactory::Page page);
	Q_SIGNAL void currentControlsChanged(int page);

private:
	QWidget* createControl(QAction* action) const;

private:
	QMap<PageFactory::Page, QList<QWidget*>> m_controls;
	QMap<QAction*, QWidget*>  m_controlsByAction;
	PageFactory::Page m_activePage;
};

}