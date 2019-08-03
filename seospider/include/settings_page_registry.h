#pragma once
#include "stdafx.h"

#include "settings_page.h"

#define TYPE_STRING(prop_page_type) QByteArray(#prop_page_type)

namespace SeoSpider
{

class ISettingsPageRegistry
{
public:
	virtual ~ISettingsPageRegistry() = default;

	virtual void registerSettingsPage(const QByteArray& pageId, SettingsPage* page) = 0;
	virtual SettingsPage* settingsPageById(const QByteArray& pageId) const = 0;

	virtual QList<QByteArray> pagesKeys() const = 0;
};

class SettingsPageRegistry : public QObject, public ISettingsPageRegistry
{
	Q_OBJECT

public: 
	SettingsPageRegistry(QObject* parent = nullptr);
	virtual ~SettingsPageRegistry();

	virtual void registerSettingsPage(const QByteArray& pageId, SettingsPage* page) override;
	virtual SettingsPage* settingsPageById(const QByteArray& pageId) const override;

	virtual QList<QByteArray> pagesKeys() const override;

private:
	Q_SLOT void settingsPageDestroyed();

private:
	QMap<QByteArray, SettingsPage*> m_settingsPages;

	bool m_deleting;
};

}