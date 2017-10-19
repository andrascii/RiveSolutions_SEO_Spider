#include "control_adaptor_check_box.h"

namespace QuickieWebBot
{

void ControlAdaptorQCheckBox::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QCheckBox*>(control));

	m_control = qobject_cast<QCheckBox*>(control);
}

QVariant ControlAdaptorQCheckBox::value() const
{
	return m_control->isChecked();
}

void ControlAdaptorQCheckBox::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::Bool);

	m_control->setChecked(val.toBool());
}

void ControlAdaptorQCheckBox::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(stateChanged(int)), page, SLOT(somethingChangedSlot())));
}

}