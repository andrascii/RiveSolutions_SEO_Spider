#pragma once

namespace SeoSpider
{

class ISettingsPage 
{
public:
	virtual void applyChanges() noexcept = 0;
	virtual void reloadSettings() noexcept = 0;
	virtual bool isAutoApply() const noexcept = 0;
	
	virtual ~ISettingsPage() = default;
};

}
