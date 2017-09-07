#include "application_settings.h"

namespace QuickieWebBot
{

ApplicationSettings& ApplicationSettings::instance()
{
	static ApplicationSettings singleton;
	return singleton;
}

QVariant ApplicationSettings::get(QString key, SettingsGroup group = SettingsGroup::General)
{
	return 0;
}

void ApplicationSettings::set(QString key, QVariant value, SettingsGroup group = SettingsGroup::General)
{

}

void ApplicationSettings::setDefaults(const QString &str)
{

}

}