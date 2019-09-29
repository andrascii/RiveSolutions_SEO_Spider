#include "stdafx.h"
#include "control_adapter_combo_box.h"

namespace SeoSpider
{

void ControlAdapterQComboBox::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<QComboBox*>(control));

	m_control = qobject_cast<QComboBox*>(control);
}

QVariant ControlAdapterQComboBox::value() const
{
	return m_control->currentData();
}

void ControlAdapterQComboBox::setValue(const QVariant& val)
{
	int textIndex = m_control->findData(val);
	m_control->setCurrentIndex(textIndex == -1 ? 0 : textIndex);
}

void ControlAdapterQComboBox::connectChangesObserver(SettingsPage* page)
{
    VERIFY(QObject::connect(m_control.data(), SIGNAL(currentIndexChanged(const QString&)), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterQComboBox::qobject()
{
	return m_control.data();
}

}