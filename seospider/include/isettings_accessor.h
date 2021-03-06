#pragma once

namespace SeoSpider
{

class ISettingsAccessor
{
public:
	virtual void saveToSettings(const QByteArray& key, const QVariant& value) noexcept = 0;
	virtual QVariant loadFromSettings(const QByteArray& key, const QVariant& defaultValue = QVariant()) const noexcept = 0;
	virtual void removeKeyFromSettings(const QByteArray& key) = 0;
	virtual QStringList allKeys() const = 0;

	virtual ~ISettingsAccessor() = default;
};

}