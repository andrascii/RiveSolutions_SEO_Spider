#include "crawler_page_info_acceptor.h"

namespace QuickieWebBot
{

CrawlerPageInfoAcceptor::CrawlerPageInfoAcceptor(QObject* parent)
	: QObject(parent)
	, m_networkAccesManager(new QNetworkAccessManager(this))
{
	VERIFY(connect(m_networkAccesManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(pageDownloaded(QNetworkReply*))));
}

void CrawlerPageInfoAcceptor::handleUrl(QUrl url)
{
	m_networkAccesManager->get(QNetworkRequest(url));
}

void CrawlerPageInfoAcceptor::parsePage()
{

}

void CrawlerPageInfoAcceptor::pageDownloaded(QNetworkReply* reply)
{
	reply->readAll();
}

}