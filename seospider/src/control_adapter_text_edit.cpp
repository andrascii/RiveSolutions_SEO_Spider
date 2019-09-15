#include "control_adapter_text_edit.h"

namespace SeoSpider
{

void ControlAdapterQPlainTextEdit::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QPlainTextEdit*>(control));

	m_control = qobject_cast<QPlainTextEdit*>(control);
}

QVariant ControlAdapterQPlainTextEdit::value() const
{
	return m_control->toPlainText();
}

void ControlAdapterQPlainTextEdit::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::String ||
		val.type() == QVariant::ByteArray ||
		val.type() == QVariant::Int);

	m_control->insertPlainText(val.toString());
}

void ControlAdapterQPlainTextEdit::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(textChanged()), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterQPlainTextEdit::qobject()
{
	return m_control.data();
}

}