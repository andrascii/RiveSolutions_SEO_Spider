#pragma once

namespace QuickieWebBot
{
	
class MenuFactory final
{
public:
	enum MenuType
	{
		ContextMenuDataCollectionRow
	};

	QMenu* create(MenuType menuType);
};

}