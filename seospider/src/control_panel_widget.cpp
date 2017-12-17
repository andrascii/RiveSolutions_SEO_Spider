#include "application.h"
#include "preferences.h"
#include "control_panel_widget.h"
#include "action_registry.h"
#include "action_keys.h"
#include "crawler.h"

namespace SeoSpider
{

ControlPanelWidget::ControlPanelWidget(QWidget* parent)
	: QFrame(parent)
{
	m_ui.setupUi(this);

	VERIFY(connect(m_ui.startOrConrinueCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::startCrawling));

	VERIFY(connect(m_ui.stopCrawlingButton, &QPushButton::clicked, this, &ControlPanelWidget::stopCrawling));

	VERIFY(connect(m_ui.clearCrawlingDataButton, &QPushButton::clicked, this, &ControlPanelWidget::clearCrawlingData));

	VERIFY(connect(m_ui.urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setUrl));

	VERIFY(connect(theApp->crawler(), SIGNAL(stateChanged(int)), this, SLOT(onCrawlerStateChanged(int))));
}

const QUrl& ControlPanelWidget::url() const noexcept
{
	return m_url;
}

void ControlPanelWidget::setUrl() const
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	theApp->preferences()->setUrl(m_ui.urlLineEdit->text());
}

void ControlPanelWidget::startCrawling() const
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	ActionRegistry::instance().globalAction(s_startCrawlerAction)->trigger();
}

void ControlPanelWidget::stopCrawling() const
{
	ActionRegistry::instance().globalAction(s_stopCrawlerAction)->trigger();
}

void ControlPanelWidget::clearCrawlingData() const
{
	ActionRegistry::instance().globalAction(s_clearCrawledDataAction)->trigger();
}

void ControlPanelWidget::onCrawlerStateChanged(int state)
{
	switch (state)
	{
		case CrawlerEngine::Crawler::StateWorking:
		{
			m_ui.clearCrawlingDataButton->setDisabled(true);

			m_ui.startOrConrinueCrawlingButton->setDisabled(true);

			break;
		}
		case CrawlerEngine::Crawler::StatePause:
		{
			m_ui.clearCrawlingDataButton->setDisabled(false); 

			m_ui.startOrConrinueCrawlingButton->setDisabled(false);

			m_ui.startOrConrinueCrawlingButton->setText(tr("Continue"));

			break;
		}
		case CrawlerEngine::Crawler::StatePending:
		{
			m_ui.clearCrawlingDataButton->setDisabled(false);

			m_ui.startOrConrinueCrawlingButton->setDisabled(false);

			m_ui.startOrConrinueCrawlingButton->setText(tr("Start"));

			break;
		}
		case CrawlerEngine::Crawler::StateSerializaton:
		case CrawlerEngine::Crawler::StateDeserializaton:
		{
			m_ui.clearCrawlingDataButton->setDisabled(true);

			m_ui.startOrConrinueCrawlingButton->setDisabled(true);

			m_ui.startOrConrinueCrawlingButton->setDisabled(true);
		}
	}
}

}