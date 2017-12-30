#include "control_adapter_spin_box.h"

namespace SeoSpider
{

void ControlAdapterQSpinBox::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QSpinBox*>(control));

	m_control = qobject_cast<QSpinBox*>(control);
}

QVariant ControlAdapterQSpinBox::value() const
{
	return m_control->value();
}

void ControlAdapterQSpinBox::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::Int);

	m_control->setValue(val.toInt());
}

void ControlAdapterQSpinBox::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(valueChanged(int)), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterQSpinBox::qobject()
{
	return m_control.data();
}

}