#include "stdafx.h"
#include "control_adapter_line_edit.h"

namespace SeoSpider
{

void ControlAdapterQLineEdit::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QLineEdit*>(control));

	m_control = qobject_cast<QLineEdit*>(control);
}

QVariant ControlAdapterQLineEdit::value() const
{
	return m_control->text();
}

void ControlAdapterQLineEdit::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::String ||
		val.type() == QVariant::ByteArray ||
		val.type() == QVariant::Int);

	m_control->setText(val.toString());
}

void ControlAdapterQLineEdit::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(textChanged(QString)), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterQLineEdit::qobject()
{
	return m_control.data();
}

}