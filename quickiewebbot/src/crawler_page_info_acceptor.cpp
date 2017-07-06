#include "crawler_page_info_acceptor.h"

namespace QuickieWebBot
{

CrawlerPageInfoAcceptor::CrawlerPageInfoAcceptor(QObject* parent)
	: QObject(parent)
	, m_networkAccesManager(new QNetworkAccessManager(this))
	, m_element(std::make_shared<WebSiteAnalyseElement>())
{
	VERIFY(connect(m_networkAccesManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(pageDownloaded(QNetworkReply*))));
}

void CrawlerPageInfoAcceptor::handleUrl(QUrl url)
{
	m_networkAccesManager->get(QNetworkRequest(url));
}

void CrawlerPageInfoAcceptor::parsePage(QString const& htmlPage) noexcept
{
	GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, htmlPage.toStdString().c_str(), htmlPage.size());
	GumboNode* root = output->root;

	assert(root->type == GUMBO_NODE_ELEMENT);
	assert(root->v.element.children.length >= 2);

	GumboNode* head = findGumboNode(root, GUMBO_TAG_HEAD);
	GumboNode* title = findGumboNode(head, GUMBO_TAG_TITLE);

	m_element->title = title->v.text.text;

	gumbo_destroy_output(&kGumboDefaultOptions, output);
}

GumboNode* CrawlerPageInfoAcceptor::findGumboNode(GumboNode* node, int gumboTagType) noexcept
{
	GumboNode* findNode = nullptr;

	const GumboVector* nodeChildren = &node->v.element.children;

	for (unsigned i = 0; i < nodeChildren->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(nodeChildren->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == gumboTagType)
		{
			findNode = child;
			break;
		}
	}

	return findNode;
}

unsigned CrawlerPageInfoAcceptor::countGumboNode(GumboNode* node, int gumboTagType) noexcept
{
	unsigned counter = 0;

	const GumboVector* nodeChildren = &node->v.element.children;

	for (unsigned i = 0; i < nodeChildren->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(nodeChildren->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == gumboTagType)
		{
			++counter;
		}
	}

	return counter;
}

void CrawlerPageInfoAcceptor::pageDownloaded(QNetworkReply* reply)
{
	QString htmlPage = reply->readAll();

	for(QPair<QByteArray, QByteArray> const& pair : reply->rawHeaderPairs())
	{
		m_element->serverResponse += pair.first + ": " + pair.second + "\n";
	}

	parsePage(htmlPage);
	
	emit addElement(thread(), m_element);
}

}