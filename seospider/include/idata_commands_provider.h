#pragma once

#include "commands.h"

namespace SeoSpider
{

class IDataCommandsProvider
{
public:
	virtual ~IDataCommandsProvider() = default;

	virtual std::vector<ICommandPointer> commandsFor(const QModelIndex& index) const = 0;
};

}