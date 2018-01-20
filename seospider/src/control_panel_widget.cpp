#include "application.h"
#include "preferences.h"
#include "control_panel_widget.h"
#include "action_registry.h"
#include "action_keys.h"
#include "crawler.h"
#include "url_line_edit.h"
#include "page_factory.h"
#include "header_controls_container.h"

namespace SeoSpider
{

ControlPanelWidget::ControlPanelWidget(QWidget* parent)
	: QFrame(parent)
	, m_startOrConrinueCrawlingAction(new QAction(tr("Start"), this))
	, m_stopCrawlingAction(new QAction(tr("Stop"), this))
	, m_clearCrawlingDataAction(new QAction(tr("Reset"), this))
{
	m_ui.setupUi(this);

	m_urlLineEdit = new UrlLineEdit(this);
	m_urlLineEdit->setObjectName(QStringLiteral("urlLineEdit"));
	m_urlLineEdit->setProperty("isValidUrl", QVariant(false));
	m_urlLineEdit->setPlaceholderText(QString("http://example.com"));

	VERIFY(connect(m_startOrConrinueCrawlingAction, &QAction::triggered, this, &ControlPanelWidget::startCrawling));
	VERIFY(connect(m_stopCrawlingAction, &QAction::triggered, this, &ControlPanelWidget::stopCrawling));
	VERIFY(connect(m_clearCrawlingDataAction, &QAction::triggered, this, &ControlPanelWidget::clearCrawlingData));
	VERIFY(connect(m_urlLineEdit, &QLineEdit::editingFinished, this, &ControlPanelWidget::setUrl));
	VERIFY(connect(theApp->crawler(), SIGNAL(stateChanged(int)), this, SLOT(onCrawlerStateChanged(int))));

	HeaderControlsContainer* store = theApp->headerControlsContainer();

	store->addWidget(m_urlLineEdit, PageFactory::SiteAuditPage);

	store->addAction(m_startOrConrinueCrawlingAction, PageFactory::SiteAuditPage);
	store->addAction(m_stopCrawlingAction, PageFactory::SiteAuditPage);
	store->addAction(m_clearCrawlingDataAction, PageFactory::SiteAuditPage);

	m_urlLineEdit->installEventFilter(this);

	VERIFY(connect(theApp->headerControlsContainer(), &HeaderControlsContainer::currentControlsChanged, 
		this, &ControlPanelWidget::onControlsChanged));
}

const Url& ControlPanelWidget::url() const noexcept
{
	return m_url;
}

bool ControlPanelWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object != qobject_cast<QObject*>(m_urlLineEdit))
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

	if (m_startOrConrinueCrawlingAction->isEnabled())
	{
		setUrl();
		startCrawling();
	}

	return false;
}

void ControlPanelWidget::setUrl() const
{
	if (!m_urlLineEdit->isUrlCorrect())
	{
		return;
	}

	Url url(Url(m_urlLineEdit->text()));

	if (url.scheme().isEmpty())
	{
		DEBUGLOG << "Corrected scheme of passed URL";

		url.setUrl("http://" + url.toString());
	}

	ActionRegistry::instance().globalAction(s_startCrawlerAction)->setData(url);
}

void ControlPanelWidget::startCrawling() const
{
	if (!m_urlLineEdit->isUrlCorrect())
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
			m_clearCrawlingDataAction->setDisabled(true);
			m_startOrConrinueCrawlingAction->setDisabled(true);

			break;
		}
		case CrawlerEngine::Crawler::StatePause:
		{
			m_clearCrawlingDataAction->setDisabled(false); 
			m_startOrConrinueCrawlingAction->setDisabled(false);
			m_startOrConrinueCrawlingAction->setText(tr("Continue"));

			break;
		}
		case CrawlerEngine::Crawler::StatePending:
		{
			m_clearCrawlingDataAction->setDisabled(false);
			m_startOrConrinueCrawlingAction->setDisabled(false);
			m_startOrConrinueCrawlingAction->setText(tr("Start"));

			break;
		}
		case CrawlerEngine::Crawler::StateSerializaton:
		case CrawlerEngine::Crawler::StateDeserializaton:
		{
			m_clearCrawlingDataAction->setDisabled(true);
			m_startOrConrinueCrawlingAction->setDisabled(true);
		}
	}
}

void ControlPanelWidget::onControlsChanged(int page)
{
	PageFactory::Page currentPage = static_cast<PageFactory::Page>(page);
	QList<QWidget*> controls = theApp->headerControlsContainer()->controls(currentPage);

	QLayoutItem *item;
	while ((item = m_ui.actionsLayout->itemAt(0))) 
	{
		if (item->widget())
		{
			item->widget()->hide();
		}

		m_ui.actionsLayout->removeItem(item);
		delete item;
	}

	foreach(QWidget* control, controls)
	{
		m_ui.actionsLayout->addWidget(control);
		control->show();
	}

	m_ui.gridLayout->update();
}
}
