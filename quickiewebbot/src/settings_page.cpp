#include "application.h"
#include "application_properties.h"
#include "settings_page.h"

namespace QuickieWebBot
{

SettingsPage::SettingsPage(bool isAutoApply, QWidget* parent)
	: QWidget(parent)
	, m_isAutoApply(isAutoApply)
{
}

void SettingsPage::applyChanges() noexcept
{
	foreach(QObject* control, findChildren<QObject*>())
	{
		if (!control->property("controlKey").isValid())
		{
			continue;
		}

		const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));

		Q_ASSERT(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
		Q_ASSERT(controlKeyString.toUtf8() == controlKeyString.toLatin1());

		const QByteArray controlKeyValue = controlKeyString.toLatin1();

		if (controlKeyValue.isEmpty())
		{
			continue;
		}

		theApp->properties()->setProperty(controlKeyValue.constData(), control->property("controlKey"));
	}
}

void SettingsPage::reloadSettings() noexcept
{

}

bool SettingsPage::isAutoApply() const noexcept
{
	return m_isAutoApply;
}

void SettingsPage::init()
{
	foreach(QObject* control, findChildren<QObject*>())
	{
		if (!control->property("controlKey").isValid())
		{
			continue;
		}

		const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));

		Q_ASSERT(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
		Q_ASSERT(controlKeyString.toUtf8() == controlKeyString.toLatin1());

		const QByteArray controlKeyValue = controlKeyString.toLatin1();

		if (controlKeyValue.isEmpty())
		{
			continue;
		}

		control->setProperty("controlKey", theApp->properties()->property(controlKeyValue.constData()));
	}
}

}