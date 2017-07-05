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

void CrawlerPageInfoAcceptor::parsePage(QString const& htmlPage) noexcept
{
	GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, htmlPage.toStdString().c_str(), htmlPage.size());
	GumboNode* root = output->root;

	assert(root->type == GUMBO_NODE_ELEMENT);
	assert(root->v.element.children.length >= 2);

	const GumboVector* rootChildren = &root->v.element.children;

	auto tagFinder = [](const GumboVector* rootChildren, int tagType) -> GumboNode*
	{
		GumboNode* node = nullptr;

		for (int i = 0; i < rootChildren->length; ++i)
		{
			GumboNode* child = static_cast<GumboNode*>(rootChildren->data[i]);

			if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tagType)
			{
				node = child;
				break;
			}
		}

		return node;
	};

	GumboNode* head = tagFinder(rootChildren, GUMBO_TAG_HEAD);
	GumboNode* body = tagFinder(rootChildren, GUMBO_TAG_BODY);

	assert(head != nullptr);
	assert(body != nullptr);

	parseContent(head);
	parseMetaRefresh(head);
	parseMetaRefresh(head);
	parseRedirectedUrl(head);
	parseTitle(head);
	parseMetaDescription(head);
	parseMetaKeywords(head);

	parseFirstH1(body);
	parseSecondH1(body);
	parseFirstH2(body);
	parseSecondH2(body);

	gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void CrawlerPageInfoAcceptor::parseContent(GumboNode* head) noexcept
{
}

void CrawlerPageInfoAcceptor::parseMetaRefresh(GumboNode* head) noexcept
{
}

void CrawlerPageInfoAcceptor::parseMetaRobots(GumboNode* head) noexcept
{
}

void CrawlerPageInfoAcceptor::parseRedirectedUrl(GumboNode* head) noexcept
{
}

void CrawlerPageInfoAcceptor::parseTitle(GumboNode* head) noexcept
{
	const GumboVector* headChildren = &head->v.element.children;

	for (int i = 0; i < headChildren->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(headChildren->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == GUMBO_TAG_TITLE)
		{
			if (child->v.element.children.length != 1)
			{
				//return "<empty title>";
			}

			GumboNode* title_text = static_cast<GumboNode*>(child->v.element.children.data[0]);

			assert(title_text->type == GUMBO_NODE_TEXT || title_text->type == GUMBO_NODE_WHITESPACE);

			//return title_text->v.text.text;
		}
	}

	// title not found
}

void CrawlerPageInfoAcceptor::parseMetaDescription(GumboNode* head) noexcept
{
}

void CrawlerPageInfoAcceptor::parseMetaKeywords(GumboNode* head) noexcept
{
}

void CrawlerPageInfoAcceptor::parseFirstH1(GumboNode* body) noexcept
{
}

void CrawlerPageInfoAcceptor::parseSecondH1(GumboNode* body) noexcept
{
}

void CrawlerPageInfoAcceptor::parseFirstH2(GumboNode* body) noexcept
{
}

void CrawlerPageInfoAcceptor::parseSecondH2(GumboNode* body) noexcept
{
}

void CrawlerPageInfoAcceptor::pageDownloaded(QNetworkReply* reply)
{
	QString htmlPage = reply->readAll();
	parsePage(htmlPage);
}

}