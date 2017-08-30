#include "control_adaptor_check_box.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

void ControlAdaptorQCheckBox::setControl(QObject* control)
{
	Q_ASSERT(qobject_cast<QCheckBox*>(control));

	m_control = qobject_cast<QCheckBox*>(control);
}

QVariant ControlAdaptorQCheckBox::value() const
{
	return m_control->isChecked();
}

void ControlAdaptorQCheckBox::setValue(const QVariant& val)
{
	Q_ASSERT(val.type() == QVariant::Bool);

	m_control->setChecked(val.toBool());
}

}