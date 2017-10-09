#include "debug_info_web_page_widget.h"
#include "parsed_page.h"
#include "application.h"
#include "main_frame.h"

namespace QuickieWebBot
{

DebugInfoWebPageWidget::DebugInfoWebPageWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);
	setWindowTitle("Selected Page Debug Info");

	VERIFY(connect(GlobalWebPageSelectedNotifier::instanse(), &GlobalWebPageSelectedNotifier::pageSelected,
		this, &DebugInfoWebPageWidget::onPageSelected));

	setWindowFlags(Qt::Dialog);
}


void DebugInfoWebPageWidget::attach()
{
	DebugInfoWebPageWidget* debugInfoWebPageWidget = new DebugInfoWebPageWidget(theApp->mainFrame());
	theApp->installEventFilter(debugInfoWebPageWidget);
}

void DebugInfoWebPageWidget::onPageSelected(WebCrawler::ParsedPage* page)
{
	responseHeadersEdit->setPlainText(page->serverResponse);
	requestHeadersEdit->setPlainText("Not Implemented yet");

#ifdef QT_DEBUG

	if (page->resourceType == WebCrawler::ResourceType::ResourceHtml)
	{
		QByteArray rawHtml = qUncompress(page->rawResponse);
		rawHtmlEdit->setPlainText(rawHtml);
		webEngineView->setHtml(rawHtml, page->url);
	}
	
	// TODO: process images

#endif
}

bool DebugInfoWebPageWidget::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		const bool isControlF12Pressed = keyEvent->key() == Qt::Key_F12 && keyEvent->modifiers() & Qt::ControlModifier;

		if (isControlF12Pressed)
		{
			if (isVisible())
			{
				hide();
			}
			else
			{
				show();
			}

			return true;
		}
	}

	return QObject::eventFilter(object, event);
}

}

