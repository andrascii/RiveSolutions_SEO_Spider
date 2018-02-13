#include "control_adapter_color_selector.h"

namespace SeoSpider
{
	
void ControlAdapterColorSelector::setControl(QObject* control)
{
	DEBUG_ASSERT(qobject_cast<ColorSelector*>(control));

	m_control = qobject_cast<ColorSelector*>(control);
}

QVariant ControlAdapterColorSelector::value() const
{
	return m_control->getColor();
}

void ControlAdapterColorSelector::setValue(const QVariant& val)
{
	DEBUG_ASSERT(val.type() == QVariant::Color);

	m_control->setColor(qvariant_cast<QColor>(val));
}

void ControlAdapterColorSelector::connectChangesObserver(SettingsPage* page)
{
	VERIFY(QObject::connect(m_control.data(), SIGNAL(colorChanged()), page, SLOT(somethingChangedSlot())));
}

QObject* ControlAdapterColorSelector::qobject()
{
	return m_control.data();
}

}