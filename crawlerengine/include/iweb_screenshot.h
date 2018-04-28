#pragma once

namespace CrawlerEngine
{

class IWebScreenShot
{
public:
	virtual ~IWebScreenShot() = default;

	virtual void load(const QUrl& url) = 0;
	virtual const QPixmap& result() const = 0;
	virtual void setResult(const QPixmap& pixmap) = 0;
	virtual QObject* qobject() const noexcept = 0;

	// signal
	virtual void loaded(const QUrl& url, const QPixmap& pixmap) = 0;
};

}