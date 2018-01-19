#pragma once

#include "commands.h"
#include "idata_commands_provider.h"

namespace SeoSpider
{
	
class CommandMenu : public QMenu
{
public:
	CommandMenu(IDataCommandsProvider* dataCommandsProvider);

	void execFor(const QPoint& pos, const QModelIndex& index) const;

private:
	IDataCommandsProvider* m_dataCommandsProvider;
};

}