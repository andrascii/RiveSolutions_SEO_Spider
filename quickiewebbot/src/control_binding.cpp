#include "control_binding.h"
#include "settings_page.h"

namespace QuickieWebBot
{
	ControlBinding::ControlBinding(const QByteArray& propertyName)
		: m_propertyName(propertyName)
	{
	}

	ControlBinding::~ControlBinding()
	{
		
	}

	//template <typename UiType>
	//void ControlBinding::bind(SettingsPage<UiType>* page, QObject* control)
	//{
	//	m_control = control;
	//	m_page = page;                                                                                                                     
	//}

	
	QVariant ControlBinding::value() const
	{
		return m_control->property(m_propertyName.constData());
	}
		
	void ControlBinding::setValue(const QVariant& val)
	{
		if (!val.isValid())
		{
			return;
		}
		m_control->setProperty(m_propertyName.constData(), val);
	}
}