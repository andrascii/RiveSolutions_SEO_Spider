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

void CustomPushButton::setActive(bool active)
{
	m_isActive = active;
	update();
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

	if (m_isInFocus || m_isActive)
	{
		option.state |= QStyle::State_On | QStyle::State_HasFocus;
	}

	if (m_isActive)
	{
		option.state &= ~QStyle::State_Raised;
		option.state |= QStyle::State_Sunken;
	}
	else
	{
		option.state |= QStyle::State_Raised;
		option.state &= ~QStyle::State_Sunken;
	}

	p.drawControl(QStyle::CE_PushButton, option);
}

}