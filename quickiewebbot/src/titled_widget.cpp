#include "titled_widget.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TitledWidget::TitledWidget(QWidget* parent)
	: QFrame(parent)
	, m_titleFrame(new QFrame(this))
	, m_titleLayout(new QHBoxLayout(m_titleFrame))
	, m_layout(new QVBoxLayout(this))
{
	m_titleFrame->setObjectName("TitleFrame");

	m_titleLayout->setSpacing(0); 
	m_titleLayout->setSpacing(0);

	m_layout->setSpacing(0);
	m_layout->setMargin(0);
	m_layout->addWidget(m_titleFrame);
}

void TitledWidget::addTitleWidget(QWidget* widget, Qt::AlignmentFlag align)
{
	m_titleLayout->addWidget(widget, 0, align);
}

void TitledWidget::setContentWidget(QWidget* widget)
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