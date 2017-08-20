#include "debug_info_web_page_widget.h"
#include "page_raw.h"

namespace QuickieWebBot
{

DebugInfoWebPageWidget::DebugInfoWebPageWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);
	setWindowTitle("Selected Page Debug Info");

	QObject::connect(GlobalWebPageSelectedNotifier::instanse(), &GlobalWebPageSelectedNotifier::pageSelected,
		this, &DebugInfoWebPageWidget::onPageSelected);
}

void DebugInfoWebPageWidget::onPageSelected(WebCrawler::PageRaw* page)
{
	responseHeadersEdit->setPlainText(page->serverResponse);
	requestHeadersEdit->setPlainText("Not Implemented yet");
#ifdef DEBUG
	QByteArray rawHtml = qUncompress(page->rawHtml);
	rawHtmlEdit->setPlainText(rawHtml);
	webEngineView->setHtml(rawHtml, page->url);
#endif // DEBUG
	
}

}

