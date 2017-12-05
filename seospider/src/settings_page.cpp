#include "application.h"
#include "settings_page.h"
#include "preferences.h"
#include "icontrol_adapter.h"
#include "control_adapter_check_box.h"
#include "control_adapter_line_edit.h"
#include "control_adapter_spin_box.h"
#include "control_adapter_combo_box.h"

namespace SeoSpider
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
		DEBUGLOG << controlKey.constData() << " " << m_controlAdaptors[controlKey]->value().toString();
		theApp->preferences()->setProperty(controlKey.constData(), m_controlAdaptors[controlKey]->value());
		DEBUGLOG << theApp->preferences()->property(controlKey.constData()).toString();
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
		const QVariant propertyValue = theApp->preferences()->property(controlAdaptor.first.toLatin1().constData());
		controlAdaptor.second->setValue(propertyValue);
		DEBUGLOG << controlAdaptor.first << " " << controlAdaptor.second->value().toString();
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

		
		QVariant propertyValue = theApp->preferences()->property(controlKey.constData());

		if (!propertyValue.isValid())
		{
			ERRLOG << controlKey.constData() 
				<< "which appears in the" 
				<< control->objectName() 
				<< "is not exists in the ApplicationProperties! It will be ignored!";

			continue;
		}

		std::shared_ptr<IControlAdapter> controlAdaptor = createControlAdaptor(control);

		const QSignalBlocker blocker(controlAdaptor->qobject());

		controlAdaptor->setValue(propertyValue);

		m_controlAdaptors[controlKeyString] = controlAdaptor;
	}
}


void SettingsPage::registerMetaTypes() const
{
	qRegisterMetaType<ControlAdapterQCheckBox>();
	qRegisterMetaType<ControlAdapterQLineEdit>();
	qRegisterMetaType<ControlAdapterQSpinBox>();
	qRegisterMetaType<ControlAdapterQComboBox>();
}

void SettingsPage::somethingChangedSlot()
{
	DEBUG_ASSERT(sender()->property("controlKey").isValid());

	if (m_changedSettingsKeys.contains(sender()->property("controlKey").toString()))
	{
		return;
	}

	m_changedSettingsKeys.push_back(sender()->property("controlKey").toString());
	setSomethingChanged(true);
	emit somethingChangedSignal();
}

std::shared_ptr<IControlAdapter> SettingsPage::createControlAdaptor(QObject* control)
{
	const QMetaObject* metaObject = control->metaObject();
	const char* className = metaObject->className();
	const QByteArray controlStringType = QString{ "%1%2" }.arg("ControlAdapter").arg(className).toLatin1();
	const int type = QMetaType::type(controlStringType);

	std::shared_ptr<IControlAdapter> controlAdapter{ static_cast<IControlAdapter*>(QMetaType::create(type)) };

	DEBUG_ASSERT((controlAdapter != nullptr) && "Non registered control adaptor type.");

	controlAdapter->setControl(control);
	controlAdapter->connectChangesObserver(this);

	return controlAdapter;
}

}