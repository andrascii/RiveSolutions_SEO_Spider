#include "crawler_progress_bar.h"
#include "application.h"
#include "main_window.h"

namespace QuickieWebBot
{

CrawlerProgressBar::CrawlerProgressBar(QWidget* parent)
	: QProgressBar(parent)
{
	setMaximum(100);

	VERIFY(connect(theApp, &Application::crawlerStarted, this, &QWidget::show));
	VERIFY(connect(theApp, &Application::crawlerStopped, this, &QWidget::hide));

	VERIFY(connect(theApp->crawler(), SIGNAL(crawlingState(CrawlingState)),
		this, SLOT(calculatePercents(CrawlingState)), Qt::QueuedConnection));

	hide();
}

void CrawlerProgressBar::calculatePercents(CrawlingState state)
{
	const size_t amountLinksCount = state.pendingLinkCount + state.crawledLinkCount;

	const double percents = static_cast<double>(state.crawledLinkCount) / static_cast<double>(amountLinksCount + 1) * 100.0;

	setValue(percents ? std::floor(percents)  : percents + 1);
}

}