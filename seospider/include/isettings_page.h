#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class ISettingsPage
{
public:
	virtual void applyChanges() = 0;
	virtual void reloadSettings() = 0;
	virtual bool isAutoApply() const noexcept = 0;

	virtual ~ISettingsPage() = default;
};

}
