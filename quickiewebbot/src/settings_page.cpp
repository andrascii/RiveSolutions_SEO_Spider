#include "application.h"
#include "application_settings.h"
#include "settings_page.h"
#include "icontrol_adaptor.h"
#include "control_adaptor_check_box.h"
#include "control_adaptor_line_edit.h"
#include "control_adaptor_spin_box.h"

namespace QuickieWebBot
{

SettingsPage::SettingsPage(bool isAutoApply, QWidget* parent)
	: QFrame(parent)
	, m_isAutoApply(isAutoApply)
	, m_somethingChanged(false)
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

		DEBUG_ASSERT(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
		DEBUG_ASSERT(controlKeyString.toUtf8() == controlKeyString.toLatin1());

		const QByteArray controlKey = controlKeyString.toLatin1();

		if (controlKey.isEmpty())
		{
			continue;
		}

		if (!m_changedSettingsKeys.contains(controlKeyString))
		{
			continue;
		}

		theApp->properties()->setProperty(controlKey.constData(), m_controlAdaptors[controlKey]->value());
	}

	m_changedSettingsKeys.clear();
	setSomethingChanged(false);
}

void SettingsPage::reloadSettings() noexcept
{
	INFOLOG << "Trying reload settings for " << this->windowTitle();

	if (m_somethingChanged)
	{
		return;
	}

	for (auto controlAdaptor : m_controlAdaptors)
	{
		QVariant propertyValue = theApp->properties()->property(controlAdaptor.first.toLatin1().constData());
		controlAdaptor.second->setValue(propertyValue);
	}
		
	setSomethingChanged(false);
	INFOLOG << "Settings reloaded for " << this->windowTitle();
}


bool SettingsPage::isAutoApply() const noexcept
{
	return m_isAutoApply;
}
 
void SettingsPage::setSomethingChanged(bool val) noexcept
{
	m_somethingChanged = val;
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

		DEBUG_ASSERT(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
		DEBUG_ASSERT(controlKeyString.toUtf8() == controlKeyString.toLatin1());

		const QByteArray controlKey = controlKeyString.toLatin1();

		if (controlKey.isEmpty())
		{
			continue;
		}

		
		QVariant propertyValue = theApp->properties()->property(controlKey.constData());

		if (!propertyValue.isValid())
		{
			ERRORLOG << controlKey.constData() 
				<< "which appears in the" 
				<< control->objectName() 
				<< "is not exists in the ApplicationProperties! It will be ignored!";

			continue;
		}

		std::shared_ptr<IControlAdaptor> controlAdaptor = createControlAdaptor(control);

		controlAdaptor->setValue(propertyValue);

		m_controlAdaptors[controlKeyString] = controlAdaptor;
	}
}


void SettingsPage::registerMetaTypes()
{
	qRegisterMetaType<ControlAdaptorQCheckBox>();
	qRegisterMetaType<ControlAdaptorQLineEdit>();
	qRegisterMetaType<ControlAdaptorQSpinBox>();
}

void SettingsPage::somethingChangedSlot()
{
	DEBUG_ASSERT(sender()->property("controlKey").isValid());

	m_changedSettingsKeys.push_back(sender()->property("controlKey").toString());

	setSomethingChanged(true);
	emit somethingChangedSignal();
}

std::shared_ptr<IControlAdaptor> SettingsPage::createControlAdaptor(QObject* control)
{
	const QMetaObject* metaObject = control->metaObject();
	const char* className = metaObject->className();
	const QByteArray controlStringType = QString{ "%1%2" }.arg("ControlAdaptor").arg(className).toLatin1();
	const int type = QMetaType::type(controlStringType);

	std::shared_ptr<IControlAdaptor> controlAdaptor{ static_cast<IControlAdaptor*>(QMetaType::create(type)) };

	DEBUG_ASSERT((controlAdaptor != nullptr) && "Non registered control adaptor type.");

	controlAdaptor->setControl(control);
	controlAdaptor->connectChangesObserver(this);

	return controlAdaptor;
}

}