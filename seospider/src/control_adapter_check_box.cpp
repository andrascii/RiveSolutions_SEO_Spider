#include "control_adapter_check_box.h"

namespace SeoSpider
{

void ControlAdapterQCheckBox::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QCheckBox*>(control));

	m_control = qobject_cast<QCheckBox*>(control);
}

QVariant ControlAdapterQCheckBox::value() const
{
	return m_control->isChecked();
}

void ControlAdapterQCheckBox::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::Bool);

	m_control->setChecked(val.toBool());
}

void ControlAdapterQCheckBox::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(stateChanged(int)), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterQCheckBox::qobject()
{
	return m_control.data();
}

}