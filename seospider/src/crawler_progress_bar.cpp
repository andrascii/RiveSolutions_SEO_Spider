#include "crawler_progress_bar.h"
#include "application.h"
#include "main_window.h"
#include "crawler.h"

namespace SeoSpider
{

CrawlerProgressBar::CrawlerProgressBar(QWidget* parent)
	: QProgressBar(parent)
{
	setMaximum(100);

	VERIFY(connect(theApp->crawler(), SIGNAL(stateChanged(int)), this, SLOT(onCrawlerStateChanged(int))));

	VERIFY(connect(theApp->crawler(), SIGNAL(crawlingProgress(CrawlingProgress)),
		this, SLOT(calculatePercents(CrawlingProgress)), Qt::QueuedConnection));

	hide();
}

void CrawlerProgressBar::calculatePercents(CrawlingProgress progress)
{
	const size_t amountLinksCount = progress.pendingLinkCount + progress.crawledLinkCount;

	const double percents = static_cast<double>(progress.crawledLinkCount) / static_cast<double>(amountLinksCount + 1) * 100.0;

	setValue(percents ? std::floor(percents)  : percents + 1);

	const QString format = QStringLiteral("Scanned pages: %1").arg(progress.crawledLinkCount) + " / " +
		QStringLiteral("Found pages: %1").arg(progress.pendingLinkCount);

	setFormat(format);
}

void CrawlerProgressBar::onCrawlerStateChanged(int state)
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