#include "web_screenshot.h"

namespace CrawlerEngine
{

WebScreenShot::WebScreenShot(QObject* parent)
	: QObject(parent)
	, m_timer(new QTimer(this))
{
	m_timer->setInterval(1000);
	m_timer->setSingleShot(true);
	VERIFY(connect(m_timer, &QTimer::timeout, this, &WebScreenShot::onReadyToRenderPixmap));
}

void WebScreenShot::load(const QUrl& url)
{
	ASSERT(!m_webView);
	m_webView.reset(new QWebEngineView);
	VERIFY(connect(m_webView->page(), &QWebEnginePage::loadFinished, this, &WebScreenShot::onLoadingDone));
	m_webView->resize(1280, 1024);
	m_result = QPixmap(m_webView->size());
	m_webView->load(url);
}

const QPixmap& WebScreenShot::result() const
{
	return m_result;
}

void WebScreenShot::onLoadingDone(bool ok)
{
	if (!ok)
	{
		emit loaded(m_webView->url(), QPixmap());
		return;
	}

	m_webView->showMinimized();
	m_timer->start();
}

void WebScreenShot::onReadyToRenderPixmap()
{
	m_result = m_webView->grab();
	m_result = m_result.scaledToWidth(400, Qt::SmoothTransformation);
	emit loaded(m_webView->url(), m_result);
	m_webView.reset();
}

}
