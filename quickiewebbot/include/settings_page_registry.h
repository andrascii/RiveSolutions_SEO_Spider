#pragma once
#define TYPE_STRING(prop_page_type) QByteArray(#prop_page_type)

namespace QuickieWebBot
{

class SettingsPageRegistry 
	: private QObject
{
	Q_OBJECT

public: 
	SettingsPageRegistry(QObject* parent = nullptr);
	~SettingsPageRegistry();

	void registerSettingsPage(const QByteArray& pageId, QWidget* page);
	QWidget* settingsPageById(const QByteArray& pageId) const;

	const QList<QByteArray>& pagesKeys();

private:
	Q_SLOT void settingsPageDestroyed();

private:
	QMap<QByteArray, QWidget*> m_settingsPages;
	QList<QByteArray> m_pagesKeys;

	bool m_deleting;
};

}