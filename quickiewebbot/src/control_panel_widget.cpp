#include "application.h"
#include "preferences.h"
#include "control_panel_widget.h"
#include "action_registry.h"
#include "action_keys.h"

namespace QuickieWebBot
{

ControlPanelWidget::ControlPanelWidget(QWidget* parent)
	: QFrame(parent)
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::startCrawling));
	VERIFY(connect(m_ui.stopCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::stopCrawling));
	VERIFY(connect(m_ui.urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setUrl));
}


const QUrl& ControlPanelWidget::url() const noexcept
{
	return m_url;
}

void ControlPanelWidget::setUrl()
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	theApp->preferences()->setUrl(m_ui.urlLineEdit->text());
}

void ControlPanelWidget::startCrawling()
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	ActionRegistry::instance().globalAction(s_startCrawlerAction)->trigger();
}


void ControlPanelWidget::stopCrawling()
{
	ActionRegistry::instance().globalAction(s_stopCrawlerAction)->trigger();
}

}