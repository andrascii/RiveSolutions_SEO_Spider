#include "crawler_progress_bar.h"
#include "action_registry.h"
#include "action_keys.h"

namespace QuickieWebBot
{

CrawlerProgressBar::CrawlerProgressBar(QWidget* parent)
	: QProgressBar(parent)
	, m_urlStorage(theApp->crawler()->crawlerUrlStorage())
	, m_calculatePercentTimer(new QTimer(this))
{
	m_calculatePercentTimer->setInterval(150);
	setMaximum(100);

	ActionRegistry* actionRegistry = &ActionRegistry::instance();

	VERIFY(connect(m_calculatePercentTimer, &QTimer::timeout, this, &CrawlerProgressBar::calculatePercents));
	VERIFY(connect(actionRegistry->globalAction(s_startCrawlerAction), &QAction::triggered, this, &QWidget::show));
	VERIFY(connect(actionRegistry->globalAction(s_stopCrawlerAction), &QAction::triggered, this, &QWidget::hide));

	hide();
}

void CrawlerProgressBar::hideEvent(QHideEvent* event)
{
	m_calculatePercentTimer->stop();

	event->accept();
}

void CrawlerProgressBar::showEvent(QShowEvent* event)
{
	m_calculatePercentTimer->start();

	event->accept();
}

void CrawlerProgressBar::calculatePercents()
{
	const size_t crawledLinksCount = m_urlStorage->crawledLinksCount();
	const size_t pendingLinksCount = m_urlStorage->pendingLinksCount();
	const size_t amountLinksCount = pendingLinksCount + crawledLinksCount;

	const double percents = crawledLinksCount / (amountLinksCount + 1) * 100;

	setValue(percents ? std::floor(percents)  : percents + 1);
}

}