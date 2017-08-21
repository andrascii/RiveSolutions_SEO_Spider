#include "titled_widget.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TitledWidget::TitledWidget(QWidget* parent)
	: QFrame(parent)
{
	init();
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

void TitledWidget::init()
{
	m_titleFrame = new QFrame(this);
	m_titleLayout = new QHBoxLayout(m_titleFrame);
	m_titleLayout->setMargin(QuickieWebBotHelpers::pointsToPixels(3));
	m_titleFrame->setObjectName("TitleFrame");

	m_layout = new QVBoxLayout(this);
	m_layout->setMargin(0);

	m_layout->addWidget(m_titleFrame);
}

}