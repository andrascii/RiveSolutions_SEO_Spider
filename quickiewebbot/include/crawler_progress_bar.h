#pragma once

namespace WebCrawler
{

class CrawlerUrlStorage;

}

namespace QuickieWebBot
{

class CrawlerProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	CrawlerProgressBar(QWidget* parent = nullptr);

protected:
	virtual void hideEvent(QHideEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;

private:
	Q_SLOT void calculatePercents();

private:
	const WebCrawler::CrawlerUrlStorage* m_urlStorage;

	QTimer* m_calculatePercentTimer;
};

}
