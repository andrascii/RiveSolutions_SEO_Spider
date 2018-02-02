#pragma once

#include "commands.h"
#include "imenu_data_provider.h"

namespace SeoSpider
{

class CommandMenu : public QMenu
{
	Q_OBJECT

public:
	CommandMenu(IMenuDataProvider* menuDataProvider);
	void execFor(const QPoint& pos, const QModelIndex& index);

private slots:
	void onActionTriggered();

private:
	void initMenu(QMenu& editingMenu, const Menu& menu);

private:
	IMenuDataProvider* m_menuDataProvider;
	QMap<QAction*, ICommandPointer> m_currentCommandActions;
};

}