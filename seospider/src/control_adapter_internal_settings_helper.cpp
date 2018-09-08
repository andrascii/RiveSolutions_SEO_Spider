#include "control_adapter_internal_settings_helper.h"
#include "internal_settings_helper.h"

namespace SeoSpider
{

void ControlAdapterInternalSettingsHelper::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<InternalSettingsHelper*>(control));
	m_control = qobject_cast<InternalSettingsHelper*>(control);
}

QVariant ControlAdapterInternalSettingsHelper::value() const
{
	return m_control->value();
}

void ControlAdapterInternalSettingsHelper::setValue(const QVariant& val)
{
	m_control->setValue(val);
}

void ControlAdapterInternalSettingsHelper::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(valueChanged(const QVariant&)), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterInternalSettingsHelper::qobject()
{
	return m_control.data();
}

}