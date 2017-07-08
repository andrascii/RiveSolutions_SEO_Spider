#include "crawler_page_info_acceptor.h"

namespace QuickieWebBot
{

CrawlerPageInfoAcceptor::CrawlerPageInfoAcceptor(QObject* parent)
	: QObject(parent)
	, m_networkAccesManager(new QNetworkAccessManager(this))
	, m_pageInfo(std::make_shared<PageInfo>())
	, m_state(PendingState)
{
	VERIFY(connect(m_networkAccesManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(pageDownloaded(QNetworkReply*))));
}

const std::vector<QUrl>& CrawlerPageInfoAcceptor::pageUrlList() const noexcept
{
	return m_pageUrlList;
}

void CrawlerPageInfoAcceptor::handlePage(QUrl url)
{
	m_state.store(WorkingState);
	m_networkAccesManager->get(QNetworkRequest(url));
}

void CrawlerPageInfoAcceptor::parsePageUrlList(const GumboNode* node) noexcept
{
	if (!node || (node && node->type != GUMBO_NODE_ELEMENT))
	{
		return;
	}

	GumboAttribute* href = nullptr;

	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href")))
	{
		m_pageUrlList.push_back(QUrl(href->value));
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned int i = 0; i < children->length; ++i)
	{
		parsePageUrlList(static_cast<const GumboNode*>(children->data[i]));
	}
}

void CrawlerPageInfoAcceptor::parsePage(const QString& htmlPage) noexcept
{
	GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, htmlPage.toStdString().c_str(), htmlPage.size());

	const GumboNode* head = firstSubNode(output->root, GUMBO_TAG_HEAD);
	const GumboNode* body = firstSubNode(output->root, GUMBO_TAG_BODY);

	parsePageTitle(head);
	parsePageMeta(head);
	parsePageUrlList(output->root);

	gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void CrawlerPageInfoAcceptor::parsePageTitle(const GumboNode* head) noexcept
{
	assert(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	GumboNode* title = firstSubNode(head, GUMBO_TAG_TITLE);

	if (title && title->v.element.children.length > 0)
	{
		m_pageInfo->title = nodeText(static_cast<GumboNode*>(title->v.element.children.data[0]));
	}
}

void CrawlerPageInfoAcceptor::parsePageMeta(const GumboNode* head) noexcept
{
	assert(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	std::vector<GumboNode*> metaTags = allSubNodes(head, GUMBO_TAG_META);

	for (unsigned i = 0; i < metaTags.size(); ++i)
	{
		GumboAttribute* contentAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "content");

		if (!contentAttribute)
		{
			continue;
		}

		GumboAttribute* metaHttpEquivAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "http-equiv");
		GumboAttribute* metaNameAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "name");

		assert(metaHttpEquivAttribute && !metaNameAttribute || 
			!metaHttpEquivAttribute && metaNameAttribute ||
			!metaHttpEquivAttribute && !metaNameAttribute);

		if (metaHttpEquivAttribute)
		{
			if (metaHttpEquivAttribute->value == "Content-Type")
			{
				m_pageInfo->content = contentAttribute->value;
			}

			if (metaHttpEquivAttribute->value == "refresh")
			{
				m_pageInfo->metaRefresh = contentAttribute->value;
			}
		}

		if (metaNameAttribute)
		{
			if (metaNameAttribute->value == "description")
			{
				m_pageInfo->metaDescription = contentAttribute->value;
			}

			if (metaNameAttribute->value == "keywords")
			{
				m_pageInfo->metaKeywords = contentAttribute->value;
			}

			if (metaNameAttribute->value == "robots")
			{
				m_pageInfo->metaRobots = contentAttribute->value;
			}
		}
	}
}

unsigned CrawlerPageInfoAcceptor::countChildren(const GumboNode* node, GumboTag tag) const noexcept
{
	assert(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	unsigned counter = 0;

	for (unsigned i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			++counter;
		}
	}

	return counter;
}

GumboNode* CrawlerPageInfoAcceptor::firstSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent) const noexcept
{
	assert(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	GumboNode* searchingNode = nullptr;

	assert(startIndexWhithinParent <= children->length);

	for (unsigned i = startIndexWhithinParent; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			searchingNode = child;
			break;
		}
	}

	return searchingNode;
}

std::vector<GumboNode*> CrawlerPageInfoAcceptor::allSubNodes(const GumboNode* node, GumboTag tag) const noexcept
{
	assert(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	std::vector<GumboNode*> nodes;

	for (unsigned i = 0; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			nodes.push_back(child);
		}
	}

	return nodes;
}

QString CrawlerPageInfoAcceptor::nodeText(const GumboNode* node) const noexcept
{
	assert(node->type == GUMBO_NODE_TEXT || node->type == GUMBO_NODE_WHITESPACE);

	return node->v.text.text;
}

void CrawlerPageInfoAcceptor::pageDownloaded(QNetworkReply* reply)
{
	QByteArray htmlPage = reply->readAll();
	parsePage(htmlPage);

	for(const QPair<QByteArray, QByteArray>& pair : reply->rawHeaderPairs())
	{
		m_pageInfo->serverResponse += pair.first + ": " + pair.second + "\n";
	}

	m_pageInfo->url = reply->url();
	
	emit pageParsed(thread(), m_pageInfo);
	m_state.store(PendingState);
}

}