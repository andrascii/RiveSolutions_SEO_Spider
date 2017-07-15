#pragma once

namespace QuickieWebBot
{

class PropertyObject : public QObject
{
	Q_OBJECT

public:
	virtual void attachPropertyObject(PropertyObject* object);

	virtual void notifyObjectChanged(PropertyObject* object, const QByteArray& propertyName, const QVariant& value);
	


private:
	std::set<PropertyObject*> m_objectListeners;
};

}
