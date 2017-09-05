#include "control_adaptor_combo_box.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

	void ControlAdaptorQComboBox::setControl(QObject* control)
	{
		DEBUG_ASSERT(qobject_cast<QComboBox*>(control));

		m_control = qobject_cast<QComboBox*>(control);
	}

	QVariant ControlAdaptorQComboBox::value() const
	{
		return m_control->currentText();
	}

	void ControlAdaptorQComboBox::setValue(const QVariant& val)
	{
		DEBUG_ASSERT(val.type() == QVariant::String);

		m_control->setEditText(val.toString());
	}

	void ControlAdaptorQComboBox::connectChangesObserver(SettingsPage* page)
	{
		VERIFY(QObject::connect(m_control.data(), SIGNAL(editTextChanged(QString)), page, SLOT(somethingChangedSlot())));
	}

}