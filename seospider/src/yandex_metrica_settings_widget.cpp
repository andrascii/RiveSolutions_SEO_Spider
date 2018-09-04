#include "yandex_metrica_settings_widget.h"
#include "internal_settings_helper.h"

namespace SeoSpider
{

YandexMetricaSettingsWidget::YandexMetricaSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
	, m_helperControl(nullptr)
{
	m_ui.setupUi(this);

	init();

	applyChanges();
}

void YandexMetricaSettingsWidget::init()
{
	m_ui.label->installEventFilter(this);

	m_lineEdits.push_back(m_ui.ycLineEdit1);
	m_lineEdits.push_back(m_ui.ycLineEdit2);
	m_lineEdits.push_back(m_ui.ycLineEdit3);
	m_lineEdits.push_back(m_ui.ycLineEdit4);
	m_lineEdits.push_back(m_ui.ycLineEdit5);

	m_helperControl = registrateInternalHelperControl(1);

	SettingsPage::init();

	VERIFY(connect(m_helperControl, SIGNAL(valueChanged(const QVariant&)),
		this, SLOT(onInternalValueChanged(const QVariant&))));

	VERIFY(connect(m_ui.addCounterButton, SIGNAL(clicked()),
		this, SLOT(incrementHelperValue())));

	VERIFY(connect(m_ui.removeCounterButton, SIGNAL(clicked()),
		this, SLOT(decrementHelperValue())));

	onInternalValueChanged(m_helperControl->value());
}

bool YandexMetricaSettingsWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_ui.label && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.ycCheckBox1->toggle();
	}

	return false;
}

void YandexMetricaSettingsWidget::onInternalValueChanged(const QVariant& value)
{
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
		}
		else
		{
			m_lineEdits[i]->hide();
		}
	}
}

void YandexMetricaSettingsWidget::decrementHelperValue()
{
	if (m_helperControl->value().toInt() <= 1)
	{
		return;
	}

	m_helperControl->setValue(m_helperControl->value().toInt() - 1);
}

void YandexMetricaSettingsWidget::incrementHelperValue()
{
	if (m_helperControl->value().toInt() >= m_lineEdits.size())
	{
		return;
	}

	m_helperControl->setValue(m_helperControl->value().toInt() + 1);
}

}