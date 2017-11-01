#pragma once

#include "service_locator.h"
#include "settings_page_registry.h"
#include "settings_page.h"


namespace SeoSpider
{

template <typename UiType>
class SettingsPageImpl : public SettingsPage, protected UiType
{
public:
	SettingsPageImpl(bool isAutoApply = false, QWidget* parent = nullptr)
		: SettingsPage(isAutoApply, parent)
	{
		UiType::setupUi(this);

		load();
	}

	static void registerSettingsPage(const QIcon& icon, const QByteArray& pageId, SettingsPage* page)
	{
		page->setWindowIcon(icon);

		ISettingsPageRegistry* settingsPageRegistry = ServiceLocator::instance()->service<ISettingsPageRegistry>();

		settingsPageRegistry->registerSettingsPage(pageId, page);
	}

	static void registerSettingsPage(const QIcon& icon, const QByteArray& pageId)
	{
		SettingsPage* page = new SettingsPageImpl<UiType>(false, nullptr);

		SettingsPageImpl::registerSettingsPage(icon, pageId, page);
	}

private:
	void load()
	{
		init();
	}
};

}
