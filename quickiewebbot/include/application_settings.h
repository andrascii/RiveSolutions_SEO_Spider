#pragma once
#include "qsettings.h"

namespace QuickieWebBot
{

class ApplicationSettings
{
public:
	enum SettingsGroup
	{
		ProjectSettings,
		General,
		WindowsSettings
	};
	
	void setDefaults(const QString &str);
	void set(QString key, QVariant value, SettingsGroup group = SettingsGroup::General);
	QVariant get(QString key, SettingsGroup group = SettingsGroup::General);
	static ApplicationSettings& instance();

private:
	std::map<SettingsGroup, std::map<QString, QSettings*>> m_applicationSettings;
};

}
