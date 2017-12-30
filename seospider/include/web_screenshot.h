#pragma once

namespace SeoSpider
{
	
class WebScreenShot : public QObject
{
	Q_OBJECT
public:
	WebScreenShot(QObject* parent = nullptr);
	void load(const QUrl& url);
	const QPixmap& result() const;

signals:
	void loaded(const QUrl& url, const QPixmap& pixmap);

private slots:
	void onLoadingDone(bool ok);
	void onReadyToRenderPixmap();

private:
	std::unique_ptr<QWebEngineView> m_webView;
	QPixmap m_result;
	QTimer* m_timer;
};

}