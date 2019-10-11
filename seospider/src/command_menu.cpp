#include "command_menu.h"
#include "commands.h"
#include "helpers.h"

namespace SeoSpider
{

CommandMenu::CommandMenu(IMenuDataProvider* menuDataProvider)
	: m_menuDataProvider(menuDataProvider)
{
}

void CommandMenu::execFor(const QPoint& pos, const QModelIndex& index)
{
	clear();
	m_currentCommandActions.clear();

	Menu menuData = m_menuDataProvider->menuFor(index);

	if (menuData.empty())
	{
		return;
	}

	initMenu(*this, menuData);
	exec(pos);
}

void CommandMenu::onActionTriggered()
{
	QAction* action = Common::Helpers::fast_cast<QAction*>(sender());
	m_currentCommandActions[action]->execute();
}

void CommandMenu::initMenu(QMenu& editingMenu, const Menu& menuData)
{
	if (menuData.empty())
	{
		DEBUGLOG << "This Menu Data Provider does not contain any commands";
		return;
	}

	for (const Common::counted_ptr<IMenuItem>& menuItem : menuData)
	{
		switch (menuItem->type())
		{
			case IMenuItem::CommandType:
			{
				ICommandPointer command = Common::Helpers::fast_cast<const CommandMenuItem*>(menuItem.get())->command();

				QAction* action = new QAction(command->icon(), command->description(), this);
				action->setEnabled(command->canExecute());

				m_currentCommandActions[action] = command;
				editingMenu.addAction(action);

				VERIFY(connect(action, &QAction::triggered, this, &CommandMenu::onActionTriggered));

				break;
			}
			case IMenuItem::SeparatorType:
			{
				editingMenu.addSeparator();

				break;
			}
			case IMenuItem::MenuType:
			{
				editingMenu.addSeparator();

				const Menu* subMenuData = Common::Helpers::fast_cast<const Menu*>(menuItem.get());
				QMenu* subMenu = new QMenu(subMenuData->name());

				initMenu(*subMenu, *subMenuData);

				editingMenu.addMenu(subMenu);
				editingMenu.addSeparator();

				break;
			}
		}
	}
}

}