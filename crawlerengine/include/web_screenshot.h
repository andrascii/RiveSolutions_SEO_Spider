#pragma once

#include "iweb_screenshot.h"

namespace CrawlerEngine
{
	
class WebScreenShot : public QObject, public IWebScreenShot
{
	Q_OBJECT

public:
	WebScreenShot(QObject* parent);

	virtual void load(const QUrl& url) override;
	virtual const QPixmap& result() const override;

signals:
	virtual void loaded(const QUrl& url, const QPixmap& pixmap) override;

private slots:
	void onLoadingDone(bool ok);
	void onReadyToRenderPixmap();

private:
	std::unique_ptr<QWebEngineView> m_webView;
	QPixmap m_result;
	QTimer* m_timer;
};

}