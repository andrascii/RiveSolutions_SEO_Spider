#include "header_decoration_widget.h"

namespace SeoSpider
{

HeaderDecorationWidget::HeaderDecorationWidget(QWidget* parent)
	: QFrame(parent)
	, m_titleFrame(new QFrame(this))
	, m_titleLayout(new QHBoxLayout(m_titleFrame))
	, m_layout(new QVBoxLayout(this))
{
	m_titleFrame->setObjectName("TitleFrame");

	m_layout->setSpacing(0);
	m_layout->setMargin(0);
	m_layout->addWidget(m_titleFrame);
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

void HeaderDecorationWidget::setContentWidget(QWidget* widget) const
{
	if (m_layout->count() > 1)
	{
		// if content widget already added
		QWidget* contentWidget = m_layout->itemAt(m_layout->count() - 1)->widget();
		m_layout->removeWidget(contentWidget);

		contentWidget->deleteLater();
	}

	m_layout->addWidget(widget);
}

}