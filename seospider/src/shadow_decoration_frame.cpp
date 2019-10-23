#include "stdafx.h"
#include "shadow_decoration_frame.h"
#include "helpers.h"

namespace SeoSpider
{

ShadowDecorationFrame::ShadowDecorationFrame(QWidget* decoratingWidget, QWidget* parent)
	: QFrame(parent)
	, m_bodyLayout(new QHBoxLayout(this))
	, m_decoratingWidget(decoratingWidget)
{
	ASSERT(m_decoratingWidget);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint);

	m_bodyLayout->addWidget(m_decoratingWidget);

	QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect;
	shadowEffect->setBlurRadius(5);
	shadowEffect->setOffset(4.0);
	m_decoratingWidget->setGraphicsEffect(shadowEffect);
	m_decoratingWidget->adjustSize();
}

}