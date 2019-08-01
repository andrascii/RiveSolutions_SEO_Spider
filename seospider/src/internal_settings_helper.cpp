#include "stdafx.h"
#include "internal_settings_helper.h"

namespace SeoSpider
{

InternalSettingsHelper::InternalSettingsHelper(QObject* parent)
	: QObject(parent)
{
}

const QVariant& InternalSettingsHelper::value() const
{
	return m_value;
}

void InternalSettingsHelper::setValue(const QVariant& value)
{
	if (value == m_value)
	{
		return;
	}

	m_value = value;
	emit valueChanged(m_value);
}

}