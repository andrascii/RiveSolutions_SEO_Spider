#include "stdafx.h"
#include "google_analytics_settings_widget.h"
#include "internal_settings_helper.h"
#include "application.h"
#include "preferences.h"
#include "helpers.h"

namespace SeoSpider
{

GoogleAnalyticsSettingsWidget::GoogleAnalyticsSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
	, m_helperControl(nullptr)
	, m_visibleLineEditCount(0)
	, m_visibleLineEditCountAtShow(-1)
    , m_visibleLineEditCountApplied(true)
	, m_applyingChanges(false)
{
	m_ui.setupUi(this);

	init();

	applyChanges();
}

void GoogleAnalyticsSettingsWidget::init()
{
	m_ui.label->installEventFilter(this);

	m_lineEdits.push_back(m_ui.gaLineEdit1);
	m_lineEdits.push_back(m_ui.gaLineEdit2);
	m_lineEdits.push_back(m_ui.gaLineEdit3);
	m_lineEdits.push_back(m_ui.gaLineEdit4);
	m_lineEdits.push_back(m_ui.gaLineEdit5);

	m_helperControl = registrateInternalHelperControl(1);
	const int useCounterHelperCount = m_lineEdits.size();

	const QMetaMethod thisValidateHelperControlSignal = Common::Helpers::metaMethodOfSlot(this, "validateHelperControlValue()");

	for (int i = 0; i < useCounterHelperCount; ++i)
	{
		const QByteArray propertyName = QStringLiteral("searchGoogleAnalyticsCounter%1").arg(i + 1).toLatin1();

		m_useCounterBooleanHelpers.push_back(registrateInternalHelperControl(false));
		m_useCounterBooleanHelpers.back()->setProperty("controlKey", QVariant(propertyName));

		VERIFY(connect(m_useCounterBooleanHelpers.back(), SIGNAL(valueChanged(const QVariant&)),
			this, SLOT(onSearchCounterChanged(const QVariant&))));

		const QMetaMethod signal = Common::Helpers::metaMethodOfSignal(theApp->preferences(), propertyName + "Changed(bool)");
		Common::Helpers::connectMetaMethods(theApp->preferences(), signal, this, thisValidateHelperControlSignal);
	}

	SettingsPage::init();

	for (int i = 0; i < m_useCounterBooleanHelpers.size(); ++i)
	{
		disconnect(m_useCounterBooleanHelpers[i], SIGNAL(valueChanged(const QVariant&)),
			this, SLOT(onSearchCounterChanged(const QVariant&)));
	}

	m_helperControl->setValue(m_visibleLineEditCount);

	VERIFY(connect(m_helperControl, SIGNAL(valueChanged(const QVariant&)),
		this, SLOT(onInternalValueChanged(const QVariant&))));

	VERIFY(connect(m_ui.addCounterButton, SIGNAL(clicked()),
		this, SLOT(incrementHelperValue())));

	VERIFY(connect(m_ui.removeCounterButton, SIGNAL(clicked()),
		this, SLOT(decrementHelperValue())));

	onInternalValueChanged(m_helperControl->value());
	validateLineEditsEnabled();
}

bool GoogleAnalyticsSettingsWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_ui.label && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.gaCheckBox1->toggle();
	}

	return false;
}

void GoogleAnalyticsSettingsWidget::onInternalValueChanged(const QVariant& value)
{
	if (m_applyingChanges)
	{
		return;
	}
	const bool isValueTypeCorrect = value.type() == QVariant::Int;
	DEBUG_ASSERT(isValueTypeCorrect);

	if (!isValueTypeCorrect)
	{
		return;
	}

	const int intValue = value.toInt();

	for (int i = 0; i < m_lineEdits.size(); ++i)
	{
		if (i < intValue)
		{
			m_lineEdits[i]->show();
			m_useCounterBooleanHelpers[i]->setValue(true);
		}
		else
		{
			m_lineEdits[i]->hide();
			m_useCounterBooleanHelpers[i]->setValue(false);
		}
	}

	validateButtonsVisibility(intValue);
	m_visibleLineEditCountApplied = false;
}

void GoogleAnalyticsSettingsWidget::decrementHelperValue()
{
	if (m_helperControl->value().toInt() <= 1)
	{
		return;
	}

	const int currentValue = m_helperControl->value().toInt() - 1;
	m_helperControl->setValue(currentValue);
	validateButtonsVisibility(currentValue);
}

void GoogleAnalyticsSettingsWidget::incrementHelperValue()
{
	if (m_helperControl->value().toInt() >= m_lineEdits.size())
	{
		return;
	}

	const int currentValue = m_helperControl->value().toInt() + 1;
	m_helperControl->setValue(currentValue);
	validateButtonsVisibility(currentValue);
}

void GoogleAnalyticsSettingsWidget::onSearchCounterChanged(const QVariant& value)
{
	DEBUG_ASSERT(value.type() == QVariant::Bool);

	if (value.type() != QVariant::Bool)
	{
		return;
	}

	++m_visibleLineEditCount;
}


void GoogleAnalyticsSettingsWidget::validateHelperControlValue()
{
	if (m_applyingChanges)
	{
		return;
	}

	std::vector<bool> ymCountersState;

	for (int i = 0, sz = m_lineEdits.size(); i < sz; ++i)
	{
		const QByteArray propertyName = QStringLiteral("searchGoogleAnalyticsCounter%1").arg(i + 1).toLatin1();

		ymCountersState.push_back(theApp->preferences()->property(propertyName).toBool());
	}

	m_helperControl->setValue(static_cast<int>(std::count(ymCountersState.begin(), ymCountersState.end(), true)));
}

void GoogleAnalyticsSettingsWidget::validateButtonsVisibility(int currentVisibleLineEditCount)
{
	m_ui.removeCounterButton->setVisible(currentVisibleLineEditCount > 1);
	m_ui.addCounterButton->setVisible(currentVisibleLineEditCount < m_lineEdits.size());
}

void GoogleAnalyticsSettingsWidget::validateLineEditsEnabled()
{
	for (int i = 0; i < m_lineEdits.size(); ++i)
	{
		m_lineEdits[i]->setEnabled(m_ui.gaCheckBox1->isChecked());
	}

	m_ui.removeCounterButton->setEnabled(m_ui.gaCheckBox1->isChecked());
	m_ui.addCounterButton->setEnabled(m_ui.gaCheckBox1->isChecked());
}


void GoogleAnalyticsSettingsWidget::onShow()
{
	if (m_visibleLineEditCountAtShow != -1)
	{
		return;
	}

	m_visibleLineEditCountAtShow = m_helperControl->value().toInt();
}

void GoogleAnalyticsSettingsWidget::onClose()
{
	if (m_visibleLineEditCountApplied)
	{
		return;
	}

	m_helperControl->setValue(m_visibleLineEditCountAtShow);
	m_visibleLineEditCountAtShow = -1;
}

void GoogleAnalyticsSettingsWidget::applyChanges()
{
	m_applyingChanges = true;
	m_visibleLineEditCountApplied = true;
	m_visibleLineEditCountAtShow = -1;

	SettingsPage::applyChanges();
	m_applyingChanges = false;
	validateHelperControlValue();
}

}
