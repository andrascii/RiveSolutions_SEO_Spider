#pragma once

#include "iweb_screenshot.h"
#include "url.h"

namespace CrawlerEngine
{
	
class WebScreenShot : public QObject, public IWebScreenShot
{
	Q_OBJECT

public:
	WebScreenShot(QObject* parent);

	virtual void load(const QUrl& url) override;
	virtual const QPixmap& result() const override;
	virtual void setResult(const QPixmap& pixmap) override;
	virtual QObject* qobject() const noexcept override;

signals:
	virtual void loaded(const QUrl& url, const QPixmap& pixmap) override;

private slots:
	void onLoadingDone(bool ok);
	void onReadyToRenderPixmap();

private:
	std::unique_ptr<QWebEngineView> m_webView;
	QPair<Url, QPixmap> m_result;
	QTimer* m_timer;
};

}
