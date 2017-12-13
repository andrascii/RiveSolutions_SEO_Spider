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
	return m_control->currentText();
}

void ControlAdapterQComboBox::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::String);

	int textIndex = m_control->findData(val, static_cast<Qt::MatchFlags>(Qt::CaseInsensitive));
	m_control->setCurrentIndex(textIndex == -1 ? 0 : textIndex);

	//m_control->setEditText(val.toString());
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