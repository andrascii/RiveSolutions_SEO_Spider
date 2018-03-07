#include "header_decoration_widget.h"
#include "helpers.h"
#include "collapse_header_button.h"

namespace
{

constexpr int c_maxHeightPt = 45;
constexpr int c_minHeightPt = 20;

}

namespace SeoSpider
{

HeaderDecorationWidget::HeaderDecorationWidget(QWidget* parent)
	: QFrame(parent)
	, m_titleFrame(new QFrame(this))
	, m_titleLayout(new QHBoxLayout)
	, m_layout(new QVBoxLayout(this))
	, m_contentWidget(new QWidget(this))
	, m_contentLayout(new QHBoxLayout(m_contentWidget))
	, m_collapseButton(new CollapseHeaderButton(this))
	, m_collapseAnimation(nullptr)
	, m_animationFinished(true)
	, m_titleFrameCollapsed(false)
{
	QVBoxLayout* layout = new QVBoxLayout(m_titleFrame);
	layout->setSpacing(0);
	layout->setMargin(0);

	m_titleLayout->setSpacing(0);
	m_titleLayout->setMargin(0);

	layout->addLayout(m_titleLayout);
	layout->addWidget(m_collapseButton);

	m_collapseButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	m_titleFrame->setObjectName("TitleFrame");

	m_layout->setSpacing(0);
	m_layout->setMargin(0);
	
	m_contentLayout->setMargin(0);
	m_contentLayout->setSpacing(0);

	m_layout->addWidget(m_titleFrame);
	m_layout->addWidget(m_contentWidget);

	VERIFY(connect(m_collapseButton, &CollapseHeaderButton::clicked, this, &HeaderDecorationWidget::onCollapseButtonClicked));

	m_titleFrame->setMaximumHeight(Common::Helpers::pointsToPixels(c_maxHeightPt));
	m_titleFrame->setMinimumHeight(Common::Helpers::pointsToPixels(c_minHeightPt));
}

void HeaderDecorationWidget::addWidgetToHeader(QWidget* widget, Qt::AlignmentFlag align, bool last) const
{
	m_titleLayout->addWidget(widget, 0, align);

	if (last)
	{
		QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
		m_titleLayout->addSpacerItem(spacer);
	}
}

void HeaderDecorationWidget::addContentWidget(QWidget* widget)
{
	m_contentLayout->addWidget(widget);
}

void HeaderDecorationWidget::mouseReleaseEvent(QMouseEvent* event)
{
	onCollapseButtonClicked();

	QFrame::mouseReleaseEvent(event);
}

void HeaderDecorationWidget::onCollapseButtonClicked()
{
	if (!m_animationFinished)
	{
		return;
	}

	constexpr int animationDuration = 500;

	const int titleFrameSourceHeight = m_titleFrame->height();

	const int titleFrameFinalHeight = m_titleFrameCollapsed ?
		Common::Helpers::pointsToPixels(c_maxHeightPt) :
		Common::Helpers::pointsToPixels(c_minHeightPt);

	QPropertyAnimation* titleFrameAnimationMaxHeight = new QPropertyAnimation(m_titleFrame, "maximumHeight");
	titleFrameAnimationMaxHeight->setDuration(animationDuration);
	titleFrameAnimationMaxHeight->setStartValue(titleFrameSourceHeight);
	titleFrameAnimationMaxHeight->setEndValue(titleFrameFinalHeight);

	QPropertyAnimation* titleFrameAnimationMinHeight = new QPropertyAnimation(m_titleFrame, "minimumHeight");
	titleFrameAnimationMinHeight->setDuration(animationDuration);
	titleFrameAnimationMinHeight->setStartValue(titleFrameSourceHeight);
	titleFrameAnimationMinHeight->setEndValue(titleFrameFinalHeight);

	CollapseHeaderButton::ArrowDirection direction = m_collapseButton->arrowDirection() == CollapseHeaderButton::ArrowDirectionUp ?
		CollapseHeaderButton::ArrowDirectionDown :
		CollapseHeaderButton::ArrowDirectionUp;

	int rotateDegreeEndValue = 0;

	switch (direction)
	{
		case CollapseHeaderButton::ArrowDirectionUp:
		{
			rotateDegreeEndValue = 0;
			break;
		}
		case CollapseHeaderButton::ArrowDirectionDown:
		{
			rotateDegreeEndValue = 180;
			break;
		}
		default:
		{
			ASSERT(!"Undefined direction");
		}
	}

	QPropertyAnimation* rotateAnimation = new QPropertyAnimation(m_collapseButton, "rotateDegree");
	rotateAnimation->setDuration(animationDuration);
	rotateAnimation->setStartValue(m_collapseButton->rotateDegree());
	rotateAnimation->setEndValue(rotateDegreeEndValue);

	m_collapseAnimation = new QParallelAnimationGroup(this);
	m_collapseAnimation->addAnimation(titleFrameAnimationMaxHeight);
	m_collapseAnimation->addAnimation(titleFrameAnimationMinHeight);
	m_collapseAnimation->addAnimation(rotateAnimation);
	m_collapseAnimation->start();

	m_animationFinished = false;
	m_titleFrameCollapsed = !m_titleFrameCollapsed;

	VERIFY(connect(m_collapseAnimation, &QAbstractAnimation::finished, this, &HeaderDecorationWidget::onAnimationFinished));
}

void HeaderDecorationWidget::onAnimationFinished()
{
	if (m_titleFrameCollapsed)
	{
		for (int i = 0; i < m_titleLayout->count(); ++i)
		{
			QWidget* widget = m_titleLayout->itemAt(i)->widget();

			if (widget && widget->isVisible() && widget != static_cast<QWidget*>(m_collapseButton))
			{
				m_hiddenWidgets.push_back(widget);
				widget->hide();
			}
		}
	}
	else
	{
		foreach(QWidget* widget, m_hiddenWidgets)
		{
			widget->show();
		}

		m_hiddenWidgets.clear();
	}

	CollapseHeaderButton::ArrowDirection direction = m_collapseButton->arrowDirection() == CollapseHeaderButton::ArrowDirectionUp ?
		CollapseHeaderButton::ArrowDirectionDown :
		CollapseHeaderButton::ArrowDirectionUp;

	m_collapseButton->setArrowDirection(direction);

	m_animationFinished = true;

	update();
	style()->unpolish(this);
	style()->polish(this);

	m_titleFrame->update();
	m_titleFrame->style()->unpolish(m_titleFrame);
	m_titleFrame->style()->polish(m_titleFrame);
}

}