#include "property_object.h"

namespace QuickieWebBot
{

void PropertyObject::attachPropertyObject(PropertyObject* object)
{
	m_objectListeners.insert(object);
}

void PropertyObject::notifyObjectChanged(PropertyObject* object, const QByteArray& propertyName, const QVariant& value)
{
	for (PropertyObject* listenerPropertyObject : m_objectListeners)
	{
		listenerPropertyObject->notifyObjectChanged(object, propertyName, value);
	}
}

}