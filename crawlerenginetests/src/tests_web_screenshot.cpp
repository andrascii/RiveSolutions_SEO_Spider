#include "tests_web_screenshot.h"

namespace CrawlerEngineTests
{

TestsWebScreenShot::TestsWebScreenShot(QObject* parent)
	: QObject(parent)
{
}

void TestsWebScreenShot::load(const QUrl& url)
{
	emit loaded(url, m_pixmapStub);
}

const QPixmap& TestsWebScreenShot::result() const
{
	return m_pixmapStub;
}

void TestsWebScreenShot::setResult(const QPixmap& pixmap)
{
	Q_UNUSED(pixmap);
}

}