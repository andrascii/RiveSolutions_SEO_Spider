#include "property_page.h"

namespace QuickieWebBot
{

CustomControlQLineEdit::CustomControlQLineEdit(QLineEdit* control)
	: m_control(control)
{
}

void CustomControlQLineEdit::connectToPropertyPage(PropertyPage* propertyPage)
{
	//VERIFY(connect(m_control, &QLineEdit::editingFinished, propertyPage, &PropertyPage::setDataFromControl));
}

//////////////////////////////////////////////////////////////////////////

void PropertyPage::setPropertyObject(PropertyObject* object)
{
	m_object = object;

	initialize();
}

void PropertyPage::initialize()
{
	for (QObject* control : m_object->findChildren<QObject*>())
	{

	}
}

void PropertyPage::setDataFromControl(const QByteArray& propertyKey)
{

}

void PropertyPage::setDataToControl()
{

}

}