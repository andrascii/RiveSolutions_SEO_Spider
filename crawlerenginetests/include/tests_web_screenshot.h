#pragma once

#include "iweb_screenshot.h"

namespace CrawlerEngineTests
{

class TestsWebScreenShot : public QObject, public CrawlerEngine::IWebScreenShot
{
	Q_OBJECT

public:
	TestsWebScreenShot(QObject* parent);

	virtual void load(const QUrl& url) override;
	virtual const QPixmap& result() const override;
	virtual void setResult(const QPixmap& pixmap) override;

signals:
	virtual void loaded(const QUrl& url, const QPixmap& pixmap) override;

private:
	QPixmap m_pixmapStub;
};

}