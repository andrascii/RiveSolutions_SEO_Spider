#include "header_decoration_widget.h"
#include "helpers.h"
#include "collapse_header_button.h"

namespace SeoSpider
{

HeaderDecorationWidget::HeaderDecorationWidget(QWidget* parent)
	: QFrame(parent)
	, m_titleFrame(new QFrame(this))
	, m_titleLayout(new QHBoxLayout(m_titleFrame))
	, m_layout(new QVBoxLayout(this))
	, m_contentWidget(nullptr)
	, m_collapseButton(new CollapseHeaderButton(m_titleFrame))
	, m_collapseAnimation(nullptr)
	, m_animationFinished(true)
	, m_titleFrameCollapsed(false)
{
	m_titleFrame->setObjectName("TitleFrame");

	m_layout->setSpacing(0);
	m_layout->setMargin(0);
	m_layout->addWidget(m_titleFrame);

	VERIFY(connect(m_collapseButton, &CollapseHeaderButton::clicked, this, &HeaderDecorationWidget::onCollapseButtonClicked));

	m_titleLayout->addWidget(m_collapseButton);
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

void HeaderDecorationWidget::setContentWidget(QWidget* widget)
{
	if (m_layout->count() > 1)
	{
		// if content widget already added
		QWidget* contentWidget = m_layout->itemAt(m_layout->count() - 1)->widget();
		m_layout->removeWidget(contentWidget);

		contentWidget->deleteLater();
	}

	m_layout->addWidget(widget);

	m_contentWidget = widget;
}

void HeaderDecorationWidget::onCollapseButtonClicked()
{
	if (!m_animationFinished)
	{
		return;
	}

	const QRect contentWidgetSourceGeometry = m_contentWidget->geometry();

	QRect contentWidgetFinalGeometry = contentWidgetSourceGeometry;

	contentWidgetFinalGeometry.setY(m_titleFrameCollapsed ?
		contentWidgetFinalGeometry.y() + Common::Helpers::pointsToPixels(30) :
		contentWidgetFinalGeometry.y() - Common::Helpers::pointsToPixels(30)
	);

	const QRect titleFrameSourceGeometry = m_titleFrame->geometry();
	QRect titleFrameFinalGeometry = titleFrameSourceGeometry;
	titleFrameFinalGeometry.setHeight(m_titleFrameCollapsed ? Common::Helpers::pointsToPixels(40) : Common::Helpers::pointsToPixels(10));

	QPropertyAnimation* contentFrameAnimation = new QPropertyAnimation(m_contentWidget, "geometry");
	contentFrameAnimation->setDuration(500);
	contentFrameAnimation->setStartValue(contentWidgetSourceGeometry);
	contentFrameAnimation->setEndValue(contentWidgetFinalGeometry);

	QPropertyAnimation* titleFrameAnimation = new QPropertyAnimation(m_titleFrame, "geometry");
	titleFrameAnimation->setDuration(500);
	titleFrameAnimation->setStartValue(titleFrameSourceGeometry);
	titleFrameAnimation->setEndValue(titleFrameFinalGeometry);

	m_collapseAnimation = new QParallelAnimationGroup(this);
	m_collapseAnimation->addAnimation(titleFrameAnimation);
	m_collapseAnimation->addAnimation(contentFrameAnimation);
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

	m_animationFinished = true;

	update();
	style()->unpolish(this);
	style()->polish(this);

	m_titleFrame->update();
	m_titleFrame->style()->unpolish(m_titleFrame);
	m_titleFrame->style()->polish(m_titleFrame);
}

}