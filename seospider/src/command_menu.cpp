#include "command_menu.h"
#include "commands.h"

namespace SeoSpider
{

CommandMenu::CommandMenu(IDataCommandsProvider* dataCommandsProvider)
	: m_dataCommandsProvider(dataCommandsProvider)
{
}

void CommandMenu::execFor(const QPoint& pos, const QModelIndex& index) const
{
	index;
	pos;
}

}