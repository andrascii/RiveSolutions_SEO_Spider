#include "custom_push_button.h"

namespace SeoSpider
{

CustomPushButton::CustomPushButton(QWidget* parent)
	: CustomPushButton(QIcon(), QString(), parent)
{
}

CustomPushButton::CustomPushButton(const QString& text, QWidget* parent)
	: CustomPushButton(QIcon(), text, parent)
{
}

CustomPushButton::CustomPushButton(const QIcon& icon, const QString& text, QWidget* parent)
	: QPushButton(icon, text, parent)
	, m_isInFocus(false)
	, m_isActive(false)
{
}

void CustomPushButton::leaveEvent(QEvent* ev)
{
	m_isInFocus = false;
	QPushButton::leaveEvent(ev);
}

void CustomPushButton::enterEvent(QEvent* ev)
{
	m_isInFocus = true;
	QPushButton::enterEvent(ev);
}

void CustomPushButton::paintEvent(QPaintEvent*)
{
	QStylePainter p(this);
	QStyleOptionButton option;
	initStyleOption(&option);

	if (m_isInFocus)
	{
		option.state |= QStyle::State_On | QStyle::State_HasFocus;
	}

	if (isSelected())
	{
		option.state &= ~QStyle::State_Raised;
		option.state |= QStyle::State_Active | QStyle::State_HasFocus;
	}
	else
	{
		option.state |= QStyle::State_Raised;
		option.state &= ~QStyle::State_Active | QStyle::State_HasFocus;
	}

	p.drawControl(QStyle::CE_PushButton, option);
}

bool CustomPushButton::isSelected() const
{
	const QVariant selectedProperty = property("selected");

	if (selectedProperty.isValid() && selectedProperty.toBool())
	{
		return true;
	}

	return false;
}

}