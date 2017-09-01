#include "control_adaptor_spin_box.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

	void ControlAdaptorQSpinBox::setControl(QObject* control)
	{
		DEBUG_ASSERT(qobject_cast<QSpinBox*>(control));

		m_control = qobject_cast<QSpinBox*>(control);
	}

	QVariant ControlAdaptorQSpinBox::value() const
	{
		return m_control->text();
	}

	void ControlAdaptorQSpinBox::setValue(const QVariant& val)
	{
		DEBUG_ASSERT(val.type() == QVariant::Int);

		m_control->setValue(val.toInt());
	}

	void ControlAdaptorQSpinBox::connectChangesObserver(SettingsPage* page)
	{
		VERIFY(QObject::connect(m_control.data(), SIGNAL(valueChanged(int)), page, SLOT(somethingChangedSlot())));
	}

}