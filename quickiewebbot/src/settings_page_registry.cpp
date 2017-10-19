#include "settings_page_registry.h"
#include "settings_page.h"
#include "main_window.h"

namespace QuickieWebBot
{

SettingsPageRegistry::SettingsPageRegistry(QObject* parent)
	: QObject(parent)
	, m_deleting(false)
{
}

SettingsPageRegistry::~SettingsPageRegistry()
{
	m_deleting = true;

	qDeleteAll(m_settingsPages.values());
}

void SettingsPageRegistry::registerSettingsPage(const QByteArray& pageId, SettingsPage* page)
{
	DEBUG_ASSERT(m_settingsPages.contains(pageId) == false);

	VERIFY(connect(page, SIGNAL(destroyed(QObject*)), SLOT(settingsPageDestroyed())));
	
	m_settingsPages[pageId] = page;
}

SettingsPage* SettingsPageRegistry::settingsPageById(const QByteArray& pageId) const
{
	return m_settingsPages.value(pageId);
}

QList<QByteArray> SettingsPageRegistry::pagesKeys() const
{
	return m_settingsPages.keys();
}

void SettingsPageRegistry::settingsPageDestroyed()
{
	DEBUG_ASSERT(m_deleting);
}

}