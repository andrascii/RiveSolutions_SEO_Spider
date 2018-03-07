#include "collapse_header_button.h"
#include "helpers.h"
#include "svg_renderer.h"

namespace SeoSpider
{

CollapseHeaderButton::CollapseHeaderButton(QWidget* parent)
	: QToolButton(parent)
	, m_direction(ArrowDirectionUp)
	, m_pixmap(SvgRenderer::render(":/images/arrow-up.svg", 10, 10))
{
	VERIFY(connect(this, &CollapseHeaderButton::clicked, this, &CollapseHeaderButton::onClicked));
	VERIFY(connect(this, SIGNAL(rotateDegreeChanged()), this, SLOT(update())));

	m_rotateDegree = rotateDegreeByDirection(m_direction);
	setArrowDirection(m_direction);
}

CollapseHeaderButton::CollapseHeaderButton(ArrowDirection direction, QWidget* parent)
	: QToolButton(parent)
	, m_direction(direction)
	, m_pixmap(SvgRenderer::render(":/images/arrow-up.svg", 10, 10))
{
	VERIFY(connect(this, &CollapseHeaderButton::clicked, this, &CollapseHeaderButton::onClicked));
	VERIFY(connect(this, SIGNAL(rotateDegreeChanged()), this, SLOT(update())));

	m_rotateDegree = rotateDegreeByDirection(m_direction);
	setArrowDirection(m_direction);
}

int CollapseHeaderButton::rotateDegree() const noexcept
{
	return m_rotateDegree;
}

void CollapseHeaderButton::setArrowDirection(ArrowDirection direction)
{
	setRotateDegree(rotateDegreeByDirection(direction));

	m_direction = direction;
}

CollapseHeaderButton::ArrowDirection CollapseHeaderButton::arrowDirection() const noexcept
{
	return m_direction;
}

void CollapseHeaderButton::onClicked()
{
// 	m_direction = m_direction == ArrowDirectionUp ? ArrowDirectionDown : ArrowDirectionUp;
//  
// 	const int rotateDegreeEndValue = rotateDegreeByDirection(m_direction);
//  
// 	QPropertyAnimation* rotateAnimation = new QPropertyAnimation(this, "rotateDegree");
// 	rotateAnimation->setDuration(500);
// 	rotateAnimation->setStartValue(rotateDegree());
// 	rotateAnimation->setEndValue(rotateDegreeEndValue);
// 	rotateAnimation->start();
// 
// 	VERIFY(connect(rotateAnimation, &QAbstractAnimation::finished, this, &CollapseHeaderButton::onAnimationFinished));
}

void CollapseHeaderButton::setRotateDegree(int degree)
{
	ASSERT(!m_pixmap.isNull());

	m_rotateDegree = degree;

	QPixmap rotate(m_pixmap.size());

	QPainter p(&rotate);
	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.setRenderHint(QPainter::HighQualityAntialiasing);
	p.translate(rotate.size().width() / 2, rotate.size().height() / 2);
	p.rotate(m_rotateDegree);
	p.translate(-rotate.size().width() / 2, -rotate.size().height() / 2);

	p.drawPixmap(0, 0, m_pixmap);
	p.end();

	setIcon(rotate);

	emit rotateDegreeChanged();
}

void CollapseHeaderButton::onAnimationFinished()
{
	QPropertyAnimation* animation = qobject_cast<QPropertyAnimation*>(sender());

	ASSERT(animation);

	animation->deleteLater();
}

int CollapseHeaderButton::rotateDegreeByDirection(ArrowDirection direction)
{
	switch (direction)
	{
		case ArrowDirectionUp:
		{
			return 0;
		}
		case ArrowDirectionDown:
		{
			return 180;
		}
		default:
		{
			ASSERT(!"Undefined direction");
		}
	}

	return 0;
}

}