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
	initialize();
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

void ControlPanelWidget::initialize()
{
	m_ui.setupUi(this);

	QAction* stopCrawlerAction = ActionRegistry::instance().globalAction(s_stopCrawlerAction);

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::startCrawling));
	VERIFY(connect(m_ui.stopCrawlingButton, SIGNAL(clicked()), stopCrawlerAction, SLOT(trigger())));
	VERIFY(connect(m_ui.urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setUrl));
}

}