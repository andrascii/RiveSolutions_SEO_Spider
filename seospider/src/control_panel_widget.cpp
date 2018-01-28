#include "application.h"
#include "preferences.h"
#include "control_panel_widget.h"
#include "action_registry.h"
#include "action_keys.h"
#include "crawler.h"
#include "url_line_edit.h"
#include "message_box_dialog.h"

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

const CrawlerEngine::Url& ControlPanelWidget::url() const noexcept
{
	return m_url;
}

bool ControlPanelWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object != qobject_cast<QObject*>(m_ui.urlLineEdit))
	{
		return false;
	}

	if (event->type() != QEvent::KeyPress)
	{
		return false;
	}

	QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

	if (keyEvent->key() != Qt::Key_Return)
	{
		return false;
	}

	if (m_ui.startOrConrinueCrawlingButton->isEnabled())
	{
		setUrl();
		startCrawling();
	}

	return false;
}

void ControlPanelWidget::setUrl() const
{
	if (!m_ui.urlLineEdit->isUrlCorrect())
	{
		return;
	}

	CrawlerEngine::Url url(CrawlerEngine::Url(m_ui.urlLineEdit->text()));

	if (url.scheme().isEmpty())
	{
		DEBUGLOG << "Corrected scheme of passed URL";

		url.setUrl("http://" + url.toString());
	}

	ActionRegistry::instance().globalAction(s_startCrawlerAction)->setData(url);
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
	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog;
	messageBoxDialog->setWindowTitle(tr("Warning"));
	
	const QString message = tr("You have not saved the results of the scanning your site.") + QString("\n") +
		tr("Would you like to save the results and have the ability to start next time from this point?");
	
	messageBoxDialog->setMessage(message);
	messageBoxDialog->setIcon(MessageBoxDialog::WarningIcon);
	messageBoxDialog->setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);
	messageBoxDialog->show();

	auto onDialogClosed = [messageBoxDialog](int result)
	{
		if (result == QDialogButtonBox::YesRole)
		{
			ActionRegistry::instance().globalAction(s_saveFileAndClearDataAction)->trigger();
		}
		else
		{
			ActionRegistry::instance().globalAction(s_clearCrawledDataAction)->trigger();
		}

		messageBoxDialog->deleteLater();
	};

	VERIFY(connect(messageBoxDialog, &MessageBoxDialog::dialogClosed, onDialogClosed));
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
		}
	}
}

}
