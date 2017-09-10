#pragma once

namespace QuickieWebBot
{

class ISettingsAccessor
{
public:
	virtual void saveToSettings() noexcept = 0;
	virtual void loadFromSettings() noexcept = 0;
};

}