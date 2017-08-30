#include "application.h"
#include "application_properties.h"
#include "settings_page.h"
#include "icontrol_adaptor.h"
#include "control_adaptor_check_box.h"

namespace QuickieWebBot
{

SettingsPage::SettingsPage(bool isAutoApply, QWidget* parent)
	: QWidget(parent)
	, m_isAutoApply(isAutoApply)
{
}

void SettingsPage::applyChanges() noexcept
{
	foreach(QObject* control, findChildren<QObject*>())
	{
		if (!control->property("controlKey").isValid())
		{
			continue;
		}

		const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));

		Q_ASSERT(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
		Q_ASSERT(controlKeyString.toUtf8() == controlKeyString.toLatin1());

		const QByteArray controlKey = controlKeyString.toLatin1();

		if (controlKey.isEmpty())
		{
			continue;
		}

		theApp->properties()->setProperty(controlKey.constData(), m_controlAdaptors[controlKey]->value());
	}
}

void SettingsPage::reloadSettings() noexcept
{
}

bool SettingsPage::isAutoApply() const noexcept
{
	return m_isAutoApply;
}

void SettingsPage::init()
{
	registerMetaTypes();

	foreach(QObject* control, findChildren<QObject*>())
	{
		if (!control->property("controlKey").isValid())
		{
			continue;
		}

		const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));

		Q_ASSERT(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
		Q_ASSERT(controlKeyString.toUtf8() == controlKeyString.toLatin1());

		const QByteArray controlKey = controlKeyString.toLatin1();

		if (controlKey.isEmpty())
		{
			continue;
		}

		if (!theApp->properties()->property(controlKey.constData()).isValid())
		{
			ERRORLOG << controlKey.constData() 
				<< "which appears in the" 
				<< control->objectName() 
				<< "is not exists in the ApplicationProperties! It will be ignored!";

			continue;
		}

		std::shared_ptr<IControlAdaptor> controlAdaptor = createControlAdaptor(control);

		controlAdaptor->setValue(theApp->properties()->property(controlKey.constData()));

		m_controlAdaptors[controlKeyString] = controlAdaptor;
	}
}


void SettingsPage::registerMetaTypes()
{
	qRegisterMetaType<ControlAdaptorQCheckBox>();
}

std::shared_ptr<IControlAdaptor> SettingsPage::createControlAdaptor(QObject* control)
{
	const QMetaObject* metaObject = control->metaObject();
	const char* className = metaObject->className();
	const QByteArray controlStringType = QString{ "%1%2" }.arg("ControlAdaptor").arg(className).toLatin1();
	const int type = QMetaType::type(controlStringType);

	std::shared_ptr<IControlAdaptor> controlAdaptor{ static_cast<IControlAdaptor*>(QMetaType::create(type)) };

	controlAdaptor->setPropertyControl(control);

	return controlAdaptor;
}

}