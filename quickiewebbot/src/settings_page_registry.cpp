#include "settings_page_registry.h"
#include "settings_page.h"

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
	m_pagesKeys.clear();
}

void SettingsPageRegistry::registerSettingsPage(const QByteArray& pageId, SettingsPage* page)
{
	DEBUG_ASSERT(m_settingsPages.contains(pageId) == false);

	VERIFY(connect(page, SIGNAL(destroyed(QObject*)), SLOT(settingsPageDestroyed())));

	m_settingsPages[pageId] = page;
	m_pagesKeys.append(pageId);
}

SettingsPage* SettingsPageRegistry::settingsPageById(const QByteArray& pageId) const
{
	return m_settingsPages.value(pageId);
}

const QList<QByteArray>& SettingsPageRegistry::pagesKeys()
{
	return m_pagesKeys;
}

void SettingsPageRegistry::settingsPageDestroyed()
{
	DEBUG_ASSERT(m_deleting);
}

}