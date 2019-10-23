#include "stdafx.h"
#include "control_adapter_radio_button.h"

namespace SeoSpider
{

void ControlAdapterQRadioButton::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QRadioButton*>(control));

	m_control = qobject_cast<QRadioButton*>(control);
}

QVariant ControlAdapterQRadioButton::value() const
{
	return m_control->isChecked();
}

void ControlAdapterQRadioButton::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::Bool);

	m_control->setChecked(val.toBool());
}

void ControlAdapterQRadioButton::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(toggled(bool)), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterQRadioButton::qobject()
{
	return m_control.data();
}

}