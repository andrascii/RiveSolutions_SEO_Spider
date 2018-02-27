#include "application.h"
#include "crawler_status_info.h"
#include "crawler_progress_bar.h"

namespace SeoSpider
{

CrawlerStatusInfo::CrawlerStatusInfo(QWidget* parent)
	: QFrame(parent)
	, m_crawledLinksLabel(new QLabel(this))
	, m_pendingLinksLabel(new QLabel(this))
	, m_progressBar(new CrawlerProgressBar(this))
{
	VERIFY(connect(theApp->crawler(), SIGNAL(stateChanged(int)), this, SLOT(onCrawlerStateChanged(int))));

	VERIFY(connect(theApp->crawler(), SIGNAL(crawlingProgress(CrawlingProgress)),
		this, SLOT(onAboutCrawlerProgressChanged(CrawlingProgress )), Qt::QueuedConnection));

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_progressBar);
	layout->addWidget(m_crawledLinksLabel);
	layout->addWidget(m_pendingLinksLabel);
	layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding));

	hide();
}

void CrawlerStatusInfo::onAboutCrawlerProgressChanged(CrawlingProgress progress)
{
	m_crawledLinksLabel->setText(QStringLiteral("Crawled Links: %1").arg(progress.crawledLinkCount));
	m_pendingLinksLabel->setText(QStringLiteral("Pending Links: %1").arg(progress.pendingLinkCount));
}

void CrawlerStatusInfo::onCrawlerStateChanged(int state)
{
	if (state == CrawlerEngine::Crawler::StatePending)
	{
		hide();
	}

	if (state == CrawlerEngine::Crawler::StateWorking)
	{
		show();
	}
}

}