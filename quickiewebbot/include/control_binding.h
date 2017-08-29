#pragma once
#include "settings_page.h"

namespace QuickieWebBot
{

class ControlBinding
{
public:
	ControlBinding(const QByteArray& propertyName);
	virtual ~ControlBinding();
	virtual QVariant value() const;
	virtual void setValue(const QVariant& val);

	template <typename UiType>
	void bind(SettingsPage<UiType>* page, QObject* control);



private:
	QByteArray m_propertyName;
	QPointer<QObject> m_control;

	//template <typename UiType>
	//QPointer<SettingsPage<UiType*>> m_page;
};

}