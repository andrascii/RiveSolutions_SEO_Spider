#include "page_info_acceptor.h"
#include "service_locator.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"

namespace QuickieWebBot
{

PageInfoAcceptor::PageInfoAcceptor(WebCrawlerInternalUrlStorage* crawlerStorage, QueuedDownloader* queuedDownloader, QObject* parent)
	: QObject(parent)
	, m_crawlerStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
	, m_pageInfo(new PageInfo)
	, m_timerId(0)
{
}

const std::vector<QUrl>& PageInfoAcceptor::pageUrlList() const noexcept
{
	return m_pageUrlList;
}

void PageInfoAcceptor::start()
{
	m_timerId = startTimer(Common::g_minimumRecommendedTimerResolution);
	assert(m_timerId);
}

void PageInfoAcceptor::stop()
{
	assert(m_timerId);
	killTimer(m_timerId);
}

void PageInfoAcceptor::timerEvent(QTimerEvent* event)
{
	processWebPage();
}

void PageInfoAcceptor::processWebPage()
{
	QUrl url;
	QueuedDownloader::Reply response;

	if (!m_crawlerStorage->get(url))
	{
		return;
	}

	m_queuedDownloader->scheduleUrl(url);
	m_queuedDownloader->extractResponse(response, QueuedDownloader::SuspendExtractPolicy);

	parseWebPage(response.responseBody);

	m_pageInfo->url = response.url;
	m_pageInfo->serverResponse = response.responseHeaderValuePairs;

	m_crawlerStorage->saveUrlList(pageUrlList());

	emit pageParsed(m_pageInfo);
}

void PageInfoAcceptor::parseWebPageUrlList(const GumboNode* node) noexcept
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
		parseWebPageUrlList(static_cast<const GumboNode*>(children->data[i]));
	}
}

void PageInfoAcceptor::parseWebPage(const QString& htmlPage) noexcept
{
	GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, htmlPage.toStdString().c_str(), htmlPage.size());

	const GumboNode* head = firstSubNode(output->root, GUMBO_TAG_HEAD);
	const GumboNode* body = firstSubNode(output->root, GUMBO_TAG_BODY);

	parseWebPageTitle(head);
	parseWebPageMeta(head);
	parseWebPageUrlList(output->root);

	gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void PageInfoAcceptor::parseWebPageTitle(const GumboNode* head) noexcept
{
	assert(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	GumboNode* title = firstSubNode(head, GUMBO_TAG_TITLE);

	if (title && title->v.element.children.length > 0)
	{
		m_pageInfo->title = nodeText(static_cast<GumboNode*>(title->v.element.children.data[0]));
	}
}

void PageInfoAcceptor::parseWebPageMeta(const GumboNode* head) noexcept
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

unsigned PageInfoAcceptor::countChildren(const GumboNode* node, GumboTag tag) const noexcept
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

GumboNode* PageInfoAcceptor::firstSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent) const noexcept
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

std::vector<GumboNode*> PageInfoAcceptor::allSubNodes(const GumboNode* node, GumboTag tag) const noexcept
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

QString PageInfoAcceptor::nodeText(const GumboNode* node) const noexcept
{
	assert(node->type == GUMBO_NODE_TEXT || node->type == GUMBO_NODE_WHITESPACE);

	return node->v.text.text;
}

}