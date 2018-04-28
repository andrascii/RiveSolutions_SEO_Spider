#include "web_screenshot.h"

namespace CrawlerEngine
{

WebScreenShot::WebScreenShot(QObject* parent)
	: QObject(parent)
	, m_timer(new QTimer(this))
{
	m_timer->setInterval(2000);
	m_timer->setSingleShot(true);
	VERIFY(connect(m_timer, &QTimer::timeout, this, &WebScreenShot::onReadyToRenderPixmap));
}

void WebScreenShot::load(const QUrl& url)
{
	if(url == m_result.first || m_webView)
	{
		return;
	}

	m_webView.reset(new QWebEngineView);

	VERIFY(connect(m_webView->page(), &QWebEnginePage::loadFinished, this, &WebScreenShot::onLoadingDone));

	m_webView->resize(1280, 1024);

	m_result = qMakePair(url, QPixmap(m_webView->size()));

	m_webView->load(url);
}

const QPixmap& WebScreenShot::result() const
{
	return m_result.second;
}

void WebScreenShot::setResult(const QPixmap& pixmap)
{
	m_result.second = pixmap;
}

QObject* WebScreenShot::qobject() const noexcept
{
	return const_cast<WebScreenShot* const>(this);
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
	m_result.second = m_webView->grab();
	m_result.second = m_result.second.scaledToWidth(400, Qt::SmoothTransformation);
	emit loaded(m_webView->url(), m_result.second);
	m_webView.reset();
}

}
