#include "control_adaptor_line_edit.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

void ControlAdaptorQLineEdit::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QLineEdit*>(control));

	m_control = qobject_cast<QLineEdit*>(control);
}

QVariant ControlAdaptorQLineEdit::value() const
{
	return m_control->text();
}

void ControlAdaptorQLineEdit::setValue(const QVariant& val)
{
	DEBUG_ASSERT((val.type() == QVariant::String) || (val.type() == QVariant::ByteArray));

	m_control->setText(val.toString());
}

void ControlAdaptorQLineEdit::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(editingFinished()), page, SLOT(somethingChangedSlot())));
}

}