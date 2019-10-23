#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class HeaderView: public QHeaderView
{
	Q_OBJECT
public:
	HeaderView(QWidget* parent, bool showCustomizeColumnsButton);

	virtual void setModel(QAbstractItemModel* model) override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;

	void initColumns();

private:
	void updateActions();
	Q_SLOT void updateMenu();
	Q_SLOT void onActionToggled(bool enabled);

private:
	bool m_showCustomizeColumnsButton;
	QPushButton* m_button;
	QMenu* m_menu;
	QTableView* m_tableView;
	QMap<int, QAction*> m_actions;
	QMap<int, int> m_sectionSize;
};

}