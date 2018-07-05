#include "download_response.h"
#include "download_request.h"
#include "requester_wrapper.h"
#include "counted_ptr.h"
#include "ihtml_node.h"
#include "myhtml_parser.h"
#include "gumbo_html_parser.h"

using namespace CrawlerEngine;

namespace
{

class Loader : public QObject
{
public:
	Loader(IHtmlParser* htmlParser, QObject* parent = nullptr)
		: QObject(parent)
		, m_htmlParser(htmlParser)
	{
	}

	void startTest()
	{
		CrawlerRequest crawlerRequest;
		crawlerRequest.url = "http://parsers.com/index.html";
		crawlerRequest.requestType = DownloadRequestType::RequestTypeGet;

		DownloadRequest request(crawlerRequest,
			DownloadRequest::LinkStatus::LinkStatusFirstLoading,
			DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoadingNecessity,
			true
		);

		m_requester.reset(request, this, &Loader::onPageDownloaded);
		m_requester->start();
	}

private:
	void onPageDownloaded(Requester*, const DownloadResponse& response)
	{
		const Hop lastHop = response.hopsChain.lastHop();

		test(m_htmlParser, lastHop.body(), lastHop.responseHeaders());

		qApp->quit();
	}

	void test(const std::unique_ptr<IHtmlParser>& htmlParser, const QByteArray& body, const ResponseHeaders& responseHeaders)
	{
		htmlParser->parseHtmlPage(body, responseHeaders);

		IHtmlNodeCountedPtr headNode = htmlParser->firstMatchNode(IHtmlNode::TagIdHead);
		IHtmlNodeCountedPtr bodyNode = htmlParser->firstMatchNode(IHtmlNode::TagIdBody);
		EXPECT_EQ((bool)headNode, true);
		EXPECT_EQ((bool)bodyNode, true);

		if (!headNode)
		{
			return;
		}

		IHtmlNodeCountedPtr titleNode = headNode->firstMatchSubNode(IHtmlNode::TagIdTitle);
		EXPECT_EQ((bool)titleNode, true);

		if (!titleNode)
		{
			return;
		}

		EXPECT_EQ(titleNode->text(), QString("Dummy Title"));

		std::vector<IHtmlNodeCountedPtr> metaNodes = headNode->matchSubNodes(IHtmlNode::TagIdMeta);
		EXPECT_EQ(metaNodes.size(), 3);

		for (const IHtmlNodeCountedPtr& node : metaNodes)
		{
			if (node->hasAttribute("http-equiv"))
			{
				continue;
			}

			const bool hasNameAttribute = node->hasAttribute("name");
			const bool hasContentAttribute = node->hasAttribute("content");

			EXPECT_EQ(hasNameAttribute && hasContentAttribute, true);

			if (!hasNameAttribute || !hasContentAttribute)
			{
				continue;
			}

			const QString nameAttributeValue = node->attribute("name");
			const QString contentAttributeValue = node->attribute("content");

			EXPECT_EQ(nameAttributeValue == "description" || nameAttributeValue == "keywords", true);

			if (nameAttributeValue == "description")
			{
				EXPECT_EQ(contentAttributeValue, QString("Dummy Description"));
			}

			if (nameAttributeValue == "keywords")
			{
				EXPECT_EQ(contentAttributeValue, QString("Dummy Keywords"));
			}
		}

		IHtmlNodeCountedPtr h1Node = bodyNode->firstMatchSubNode(IHtmlNode::TagIdH1);
		IHtmlNodeCountedPtr h2Node = bodyNode->firstMatchSubNode(IHtmlNode::TagIdH2);
		IHtmlNodeCountedPtr spanNode = bodyNode->firstMatchSubNode(IHtmlNode::TagIdSpan);
		EXPECT_EQ((bool)h1Node, true);
		EXPECT_EQ((bool)h2Node, true);
		EXPECT_EQ((bool)spanNode, true);

		if (!h1Node || !h2Node || !spanNode)
		{
			return;
		}

		EXPECT_EQ(h1Node->text(), QString("h1 tag value"));
		EXPECT_EQ(h2Node->text(), QString("h2 tag value"));
		EXPECT_EQ(spanNode->text(), "Значение тега span");
		EXPECT_EQ(spanNode->hasAttribute("color") && spanNode->attribute("color") == QString("Arial"), true);
	}

private:
	RequesterWrapper m_requester;
	std::unique_ptr<IHtmlParser> m_htmlParser;
};

}

namespace CrawlerEngineTests
{

TEST(ParserTests, MyHtmlParserTest)
{
	TestEnvironment env;

	Loader* loader = new Loader(new MyHtmlParser, &env);
	loader->startTest();

	env.exec();
}

TEST(ParserTests, GumboHtmlParserTest)
{
	TestEnvironment env;

	Loader* loader = new Loader(new GumboHtmlParser, &env);
	loader->startTest();

	env.exec();
}

}