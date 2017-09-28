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
	
	//crushes when closing app
	//qDeleteAll(m_settingsPages.values());

	//for (auto page : m_settingsPages.values())
	//{
	//	page->deleteLater();
	//}

	for (auto a : m_settingsPages.values())
	{
		qDebug() << a->windowTitle();
		delete a;
	}

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