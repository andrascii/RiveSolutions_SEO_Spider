#include "counted_ptr.h"
#include "myhtml_parser.h"
#include "response_headers.h"
#include "html_node_model.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(XPathTests, Simple)
{
	MyHtmlParser parser;

	const QByteArray html = "<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<title>Dummy Title</title>"
		"<meta name = \"description\" content=\"Dummy Description\">"
		"</head>"
		"<body>"
		"<h1>Dummy H1</h1>"
		"<a href=\"/page-1.htm\">1</a>"
		"</body>"
		"</html>";

	parser.parseHtmlPage(html, ResponseHeaders());
	HtmlNodeModel model(&parser);

	QXmlQuery xmlQuery(QXmlQuery::XQuery10);

	QXmlNodeModelIndex startNode = model.rootIndex();
	xmlQuery.bindVariable("queryRoot", startNode);

	xmlQuery.setQuery(QString("//a/text()"));

	QString result;
	xmlQuery.evaluateTo(&result);
	EXPECT_EQ(QString("1"), result);
}

}