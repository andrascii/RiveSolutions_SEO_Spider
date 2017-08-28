#pragma once

namespace QuickieWebBot
{

class ISettingsPage 
{
public:
	virtual void applyChanges() noexcept = 0;
	virtual bool isAutoApply() const noexcept = 0;
};

}
