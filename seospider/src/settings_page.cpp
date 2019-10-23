#include "stdafx.h"
#include "application.h"
#include "settings_page.h"
#include "preferences.h"
#include "icontrol_adapter.h"
#include "control_adapter_check_box.h"
#include "control_adapter_line_edit.h"
#include "control_adapter_text_edit.h"
#include "control_adapter_spin_box.h"
#include "control_adapter_combo_box.h"
#include "control_adapter_radio_button.h"
#include "control_adapter_color_selector.h"
#include "control_adapter_internal_settings_helper.h"
#include "internal_settings_helper.h"

namespace SeoSpider
{

SettingsPage::SettingsPage(bool isAutoApply, QWidget* parent)
	: QFrame(parent)
	, m_isAutoApply(isAutoApply)
	, m_somethingChanged(false)
{
}

void SettingsPage::applyChanges()
{
	foreach(QObject* control, m_controls)
	{
		const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));

		if (!m_changedSettingsKeys.contains(controlKeyString))
		{
			continue;
		}

		const QByteArray controlKey = controlKeyString.toLatin1();

		DEBUGLOG << controlKey.constData() << " " << m_controlAdapters[controlKey]->value().toString();

		theApp->preferences()->setProperty(controlKey.constData(), m_controlAdapters[controlKey]->value());

		DEBUGLOG << controlKey.constData() << " " << theApp->preferences()->property(controlKey.constData()).toString();
	}

	m_changedSettingsKeys.clear();
	setSomethingChanged(false);
}

void SettingsPage::reloadSettings()
{
	INFOLOG << "Trying reload settings for " << windowTitle();

	if (!m_somethingChanged)
	{
		return;
	}

	for (auto controlAdapter : m_controlAdapters)
	{
		const QVariant propertyValue = theApp->preferences()->property(controlAdapter.first.toLatin1().constData());
		controlAdapter.second->setValue(propertyValue);
		DEBUGLOG << controlAdapter.first << " " << controlAdapter.second->value().toString();
	}

	setSomethingChanged(false);
	INFOLOG << "Settings reloaded for " << windowTitle();
}

bool SettingsPage::isAutoApply() const noexcept
{
	return m_isAutoApply;
}

void SettingsPage::setSomethingChanged(bool val) noexcept
{
	m_somethingChanged = val;
}

void SettingsPage::clearChangedKeys() noexcept
{
	m_changedSettingsKeys.clear();
	m_somethingChanged = false;
}

bool SeoSpider::SettingsPage::somethingChanged() const noexcept
{
	return m_somethingChanged;
}

void SettingsPage::init()
{
	registerMetaTypes();

	initControlsWithProperty();

	foreach(QObject* control, m_controls)
	{
		const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));
		const QByteArray controlKey = controlKeyString.toLatin1();

		QVariant propertyValue = theApp->preferences()->property(controlKey.constData());

		if (!propertyValue.isValid())
		{
			ERRLOG << controlKey.constData()
				<< "which appears in the"
				<< control->objectName()
				<< "is not exists in the ApplicationProperties! It will be ignored!";

			continue;
		}

		std::shared_ptr<IControlAdapter> controlAdapter = createControlAdapter(control);

		QSignalBlocker blocker(controlAdapter->qobject());

		// we must not block signals from InternalSettingsHelper controls at the loading stage
		if (dynamic_cast<ControlAdapterInternalSettingsHelper*>(controlAdapter.get()))
		{
			blocker.unblock();
		}

		controlAdapter->setValue(propertyValue);

		m_controlAdapters[controlKeyString] = controlAdapter;
	}
}

InternalSettingsHelper* SettingsPage::registrateInternalHelperControl(const QVariant& initialValue)
{
	InternalSettingsHelper* helper = new InternalSettingsHelper(this);
	helper->setValue(initialValue);

	return helper;
}

void SettingsPage::registerMetaTypes() const
{
    qRegisterMetaType<ControlAdapterQCheckBox>();
	qRegisterMetaType<ControlAdapterQLineEdit>();
	qRegisterMetaType<ControlAdapterQPlainTextEdit>();
	qRegisterMetaType<ControlAdapterQSpinBox>();
	qRegisterMetaType<ControlAdapterQComboBox>();
	qRegisterMetaType<ControlAdapterQRadioButton>();
	qRegisterMetaType<ControlAdapterColorSelector>(); //"ControlAdapterSeoSpider::ColorSelector"
	qRegisterMetaType<ControlAdapterInternalSettingsHelper>(); //"ControlAdapterSeoSpider::InternalSettingsHelper"
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

std::shared_ptr<IControlAdapter> SettingsPage::createControlAdapter(QObject* control)
{
	const QMetaObject* metaObject = control->metaObject();
    const QString typeName = metaObject->className();
    QList<QString> typeParts = typeName.split("::", QString::SkipEmptyParts);
    
    QByteArray controlStringType;
    
    if (typeParts.size() == 1)
    {
        const QString typeString = typeParts[0];
        controlStringType = QString{ "SeoSpider::%1%2" }.arg("ControlAdapter").arg(typeString).toLatin1();
    }
    else if (typeParts.size() == 2)
    {
        const QString namespaceString = typeParts[0];
        const QString typeString = typeParts[1];
        qDebug() << "namespace: " << namespaceString << "\ntype: " << typeString;
        controlStringType = QString{ "%1::%2%3" }.arg(namespaceString).arg("ControlAdapter").arg(typeString).toLatin1();
    }
    else
    {
        ASSERT(!"Trying to create undefined type");
    }
    
    qDebug() << "try find: " << controlStringType;
    
	const int type = QMetaType::type(controlStringType);
	std::shared_ptr<IControlAdapter> controlAdapter{ static_cast<IControlAdapter*>(QMetaType::create(type)) };

	DEBUG_ASSERT(controlAdapter != nullptr && "Non registered control adapter type.");

	controlAdapter->setControl(control);
	controlAdapter->connectChangesObserver(this);

	return controlAdapter;
}

void SettingsPage::initControlsWithProperty()
{
	foreach(QObject* control, findChildren<QObject*>())
	{
		if (control->property("notProduction").isValid())
		{
			QWidget* controlWidget = qobject_cast<QWidget*>(control);

			DEBUG_ASSERT(controlWidget);

			if (controlWidget)
			{
				controlWidget->hide();
			}
		}

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

		m_controls.push_back(control);
	}
}

}
