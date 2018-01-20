#pragma once

#include "commands.h"
#include "idata_commands_provider.h"

namespace SeoSpider
{
	
class CommandMenu : public QMenu
{
	Q_OBJECT

public:
	CommandMenu(IDataCommandsProvider* dataCommandsProvider);
	void execFor(const QPoint& pos, const QModelIndex& index);

private slots:
	void onActionTriggered();

private:
	IDataCommandsProvider* m_dataCommandsProvider;
	QMap<QAction*, ICommandPointer> m_currentCommandActions;
};

}