#pragma once

#include "property_object.h"

namespace QuickieWebBot
{

class PropertyPage;
	
class CustomControl : public QObject
{
	Q_OBJECT

public:
	virtual void connectToPropertyPage(PropertyPage* propertyPage) = 0;
};

class CustomControlQLineEdit : public CustomControl
{
public:
	CustomControlQLineEdit(QLineEdit* control);
	virtual void connectToPropertyPage(PropertyPage* propertyPage) override;

private:
	QLineEdit* m_control;
};

class PropertyPage : public QObject
{
	Q_OBJECT

public:
	void setPropertyObject(PropertyObject* object);

	void initialize();

	Q_SLOT void setDataFromControl(const QByteArray& propertyKey);
	Q_SLOT void setDataToControl();

private:
	PropertyObject* m_object;

	std::map<PropertyObject*, QByteArray> m_bindings;

	std::set<QByteArray> m_propertyKeys;
};

}
