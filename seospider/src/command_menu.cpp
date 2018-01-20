#include "command_menu.h"
#include "commands.h"
#include "helpers.h"

namespace SeoSpider
{

CommandMenu::CommandMenu(IDataCommandsProvider* dataCommandsProvider)
	: m_dataCommandsProvider(dataCommandsProvider)
{
}

void CommandMenu::execFor(const QPoint& pos, const QModelIndex& index)
{
	clear();
	m_currentCommandActions.clear();

	std::vector<ICommandPointer> commands = m_dataCommandsProvider->commandsFor(index);

	for (const ICommandPointer& command : commands)
	{
		QAction* action = new QAction(command->icon(), command->description(), this);
		m_currentCommandActions[action] = command;
		addAction(action);

		VERIFY(connect(action, &QAction::triggered, this, &CommandMenu::onActionTriggered));
	}

	exec(pos);
}

void CommandMenu::onActionTriggered()
{
	QAction* action = Common::Helpers::fast_cast<QAction*>(sender());
	m_currentCommandActions[action]->execute();
}

}