#pragma once

#include "isettings_page.h"
#include "application_properties.h"
#include "application.h"
#include "service_locator.h"
#include "settings_page_registry.h"
#include "settings_page.h"

namespace QuickieWebBot
{

template <typename UiType>
class SettingsPageImpl
	: public SettingsPage
	, protected UiType
{
public:
	SettingsPageImpl(bool isAutoApply = false, QWidget* parent = nullptr)
		: SettingsPage(isAutoApply, parent)
	{
		UiType::setupUi(this);

		load();
	}

protected:
	static void registerSettingsPage(const QByteArray& pageId, SettingsPage* page)
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();
		SettingsPageRegistry* settingsPageRegistry = serviceLocator->service<SettingsPageRegistry>();

		settingsPageRegistry->registerSettingsPage(pageId, page);
	}

	template <typename NewUiType>
	static void registerSettingsPage(const QByteArray& pageId)
	{
		SettingsPage* page = new SettingsPageImpl<NewUiType>(false, nullptr);

		SettingsPageImpl::registerSettingsPage(pageId, page);
	}

private:
	void load()
	{
		init();
	}
};

}
