#include "application.h"
#include "crawler_status_info.h"

namespace SeoSpider
{

CrawlerStatusInfo::CrawlerStatusInfo(QWidget* parent)
	: QFrame(parent)
	, m_crawledLinksLabel(new QLabel(this))
	, m_pendingLinksLabel(new QLabel(this))
{
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::crawlerStarted, this, &QWidget::show));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::crawlerStopped, this, &QWidget::hide));

	VERIFY(connect(theApp->crawler(), SIGNAL(crawlingProgress(CrawlingProgress)),
		this, SLOT(onAboutCrawlerProgressChanged(CrawlingProgress )), Qt::QueuedConnection));

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_crawledLinksLabel);
	layout->addWidget(m_pendingLinksLabel);

	hide();
}

void CrawlerStatusInfo::onAboutCrawlerProgressChanged(CrawlingProgress progress)
{
	m_crawledLinksLabel->setText(QStringLiteral("Crawled Links: %1").arg(progress.crawledLinkCount));
	m_pendingLinksLabel->setText(QStringLiteral("Pending Links: %1").arg(progress.pendingLinkCount));
}

}